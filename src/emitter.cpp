#include "emitter.h"
#include "converter_factory.h"
#include "reflection.h"
#include "util_numeric.h"
#include "util_string.h"
#include "yaml.h"

#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

YAML::Emitter::Emitter()
{
  callbacks.m_error = error_callback;
  callbacks.m_user_data = this;
  evt_handler = std::make_unique<ryml::EventHandlerTree>(callbacks);

  init_converters();
}

void YAML::Emitter::init_converters()
{
  // Get converters by type
  type_converters = factory.create_converter_set();

  // Build tag lookup table
  for (const auto& pair : type_converters) {
    if (pair.second) {
      tag_converters[pair.second->get_tag()] = pair.second.get();
    }
  }
}

void YAML::Emitter::error_callback(const char* msg, size_t len, ryml::Location loc, void* user_data)
{
  ryml::csubstr error_msg(msg, len);

  // Strip "ERROR: " prefix if present
  const ryml::csubstr strip_error_prefix = "ERROR: ";
  if (error_msg.begins_with(strip_error_prefix)) {
    error_msg = error_msg.sub(strip_error_prefix.len);
  }

  // Get first line of error message
  size_t newline_pos = error_msg.find('\n');
  if (newline_pos != ryml::substr::npos) {
    error_msg = error_msg.sub(0, newline_pos);
  }

  // Get the emitter instance through user_data
  auto* emitter = static_cast<Emitter*>(user_data);
  if (!emitter) {
    throw YAMLException(from_ryml_str(error_msg));
  }

  // Set error in this specific emitter  instance
  emitter->current_result = YAMLResult::error(
          from_ryml_str(error_msg),
          loc.line,
          loc.col);

  throw YAMLException(emitter->current_result->get_error());
}

void YAML::Emitter::reset()
{
  tree.clear();
  current_result = YAMLResult::success(Variant());
  current_style = YAMLStyle::View::create_view();
}

VariantConverter* YAML::Emitter::get_converter_for_type(Variant::Type type) const
{
  auto it = type_converters.find(type);
  return it != type_converters.end() ? it->second.get() : nullptr;
}

VariantConverter* YAML::Emitter::get_converter_for_tag(const String& tag) const
{
  auto it = tag_converters.find(tag);
  return it != tag_converters.end() ? it->second : nullptr;
}

Ref<YAMLResult> YAML::Emitter::emit(const Variant& input, const YAMLStyle::View& style)
{
  try {
    reset();
    current_style = style;

    // Emit value into tree
    emit_value(tree.rootref(), input, style);

    // Check for errors that occurred during emission
    if (current_result->has_error()) {
      return current_result;
    }

    // Convert to YAML string
    std::string yaml_str = ryml::emitrs_yaml<std::string>(tree);
    return YAMLResult::success(String::utf8(yaml_str.c_str(), yaml_str.length()), nullptr);
  } catch (const YAMLException& e) {
    return YAMLResult::error(e.get_godot_message());
  } catch (const std::exception& e) {
    return YAMLResult::error(e.what());
  } catch (...) {
    return YAMLResult::error("Unknown error occurred during emission");
  }
}

void YAML::Emitter::emit_value(ryml::NodeRef& node, const Variant& value, const YAMLStyle::View& style)
{
  switch (value.get_type()) {
    case Variant::NIL:
      emit_nil(node);
      break;

    case Variant::BOOL:
      emit_bool(node, value);
      break;

    case Variant::INT:
    case Variant::FLOAT:
      emit_number(node, value, style);
      break;

    case Variant::STRING:
      emit_string(node, value, style);
      break;

    case Variant::ARRAY:
      emit_array(node, value, style);
      break;

    case Variant::DICTIONARY:
      emit_dictionary(node, value, style);
      break;

    case Variant::OBJECT: {
      Object* obj = value.operator Object*();
      if (obj) {
        emit_object(node, obj, style);
      } else {
        emit_nil(node);
      }
      break;
    }

    default: {
      VariantConverter* converter = get_converter_for_type(value.get_type());
      if (converter) {
        node.set_val_tag(to_ryml_str(converter->get_full_tag()));
        converter->encode(node, value, style);
      } else {
        String type_name = Variant::get_type_name(value.get_type());
        UtilityFunctions::push_warning(vformat("Unsupported type: %s", type_name));
        emit_nil(node);
      }
      break;
    }
  }

  // Add custom tags last
  if (style.is_valid() && !style.get_custom_settings().is_empty() && style.get_custom_settings().has("tag") && !node.has_val_tag()) {
    String tag = style.get_custom_settings()["tag"];
    if (!tag.is_empty()) {
      node.set_val_tag(to_ryml_str("!" + tag));
    }
  }
}

void YAML::Emitter::emit_nil(ryml::NodeRef& node)
{
  ryml::csubstr null = {};
  node << null;
}

void YAML::Emitter::emit_bool(ryml::NodeRef& node, bool value)
{
  node << (value ? "true" : "false");
}

void YAML::Emitter::emit_number(ryml::NodeRef& node, const Variant& value, const YAMLStyle::View& style)
{
  YAMLStyle::NumberFormat format = style.is_valid() ? style.get_number_format() : YAMLStyle::NUM_DECIMAL;

  if (value.get_type() == Variant::INT) {
    int64_t int_val = static_cast<int64_t>(value.operator int64_t());
    node << int_to_string(int_val, format);
  } else {
    double float_val = static_cast<double>(value.operator double());
    node << float_to_string(float_val, format);
  }
}

void YAML::Emitter::emit_string(ryml::NodeRef& node, const String& value, const YAMLStyle::View& style)
{
  if (value.is_empty()) {
    node << ryml::csubstr {};
    return;
  }

  // First handle explicit style settings if provided
  if (style.is_valid()) {
    style.apply_scalar_style(node);
    style.apply_quote_style(node);
  } else {
    // TODO: bring this back with sensible defaults
    // // Auto-detect appropriate scalar style
    // if (value.contains("\n")) {
    //   node |= ryml::BLOCK;
    //   // If the string has significant whitespace or ends with newlines,
    //   // use literal style, otherwise use folded
    //   if (value.ends_with("\n") || value.contains("  ")) {
    //     node |= ryml::VAL_LITERAL;
    //   } else {
    //     node |= ryml::VAL_FOLDED;
    //   }
    // } else if (needs_block_style(value)) {
    //   node |= ryml::BLOCK;
    // }
    // // Auto-detect quotes for non-block strings
    // if (!node.is_block()) {
    //   if (needs_quotes(value)) {
    //     node |= ryml::VAL_DQUO;
    //   }
    // }
  }

  node << to_ryml_str(value);
}

void YAML::Emitter::emit_array(ryml::NodeRef& node, const Array& array, const YAMLStyle::View& style)
{
  node |= ryml::SEQ;

  if (array.size() == 0) {
    return;
  }

  if (style.is_valid() && style.get_container_form() == YAMLStyle::FORM_SEQ) {
    style.apply_flow_style(node);
  }

  // Get shared item style if available
  YAMLStyle::View shared_style = style.is_valid() ? style.get_child("_items") : YAMLStyle::View();

  for (int i = 0; i < array.size(); i++) {
    // Check for individual item style, fall back to shared style
    YAMLStyle::View item_style;
    if (style.is_valid()) {
      item_style = style.get_child(String::num_int64(i));
      if (!item_style.is_valid()) {
        item_style = shared_style;
      }
    }
    emit_value(node.append_child(), array[i], item_style);
  }
}

void YAML::Emitter::emit_dictionary(ryml::NodeRef& node, const Dictionary& dict, const YAMLStyle::View& style)
{
  node |= ryml::MAP;
  Array keys = dict.keys();

  for (int i = 0; i < keys.size(); i++) {
    String key_str = String(keys[i]); // Convert key to string
    ryml::csubstr key_view = to_ryml_str(key_str); // Create view into string data

    // DANGER: key_str might be destroyed while key_view still points to its data
    ryml::NodeRef child = node.append_child();
    child << ryml::key(key_view); // Using potentially dangling pointer!

    emit_value(child, dict[keys[i]], style);
  }
}

void YAML::Emitter::emit_object(ryml::NodeRef& node, const Object* obj, const YAMLStyle::View& style)
{
  // Set class tag
  String class_name = obj->get_class();
  node.set_val_tag(to_ryml_str("!" + class_name));

  // Handle resources specially
  const Resource* res = Object::cast_to<const Resource>(obj);
  if (res) {
    emit_resource(node, res, style);
    return;
  }

  // For other objects, emit all properties
  node |= ryml::MAP;
  emit_object_properties(node, obj, style);
}

void YAML::Emitter::emit_resource(ryml::NodeRef& node, const Resource* res, const YAMLStyle::View& style)
{
  // Check if resource has a path and no local modifications
  if (!res->get_path().is_empty()) {
    String path = res->get_path();
    if (ProjectSettings::get_singleton()->localize_path(path) == path) {
      // No local modifications - just emit the path
      node << path.utf8().get_data();
      return;
    }
  }

  // Resource has local modifications - emit as map with path and modified properties
  node |= ryml::MAP;

  // Store path if exists
  if (!res->get_path().is_empty()) {
    ryml::NodeRef path_node = node.append_child();
    path_node << ryml::key(to_ryml_str("__path__"));
    path_node << res->get_path().utf8().get_data();
  }

  emit_object_properties(node, res, style);
}

void YAML::Emitter::emit_object_properties(ryml::NodeRef& node, const Object* obj, const YAMLStyle::View& style)
{
  Dictionary properties = ObjectReflection::get_object_properties(obj);
  Array prop_names = properties.keys();

  for (int i = 0; i < prop_names.size(); i++) {
    String prop_name = prop_names[i];
    Dictionary prop_info = properties[prop_name];

    if (should_serialize_property(prop_info)) {
      Variant value = obj->get(prop_name);
      emit_property_value(node, prop_name, value,
              style.is_valid() ? style.get_child(prop_name) : YAMLStyle::View());
    }
  }
}

void YAML::Emitter::emit_property_value(ryml::NodeRef& node, const String& prop_name,
        const Variant& value, const YAMLStyle::View& style)
{
  ryml::NodeRef child = node.append_child();
  child << ryml::key(to_ryml_str(prop_name));
  emit_value(child, value, style);
}

bool YAML::Emitter::should_serialize_property(const Dictionary& prop_info) const
{
  // Sometimes we deal with empty objects
  if (!prop_info.has("value") || !prop_info.has("type") || !prop_info.has("usage")) {
    return false;
  }

  // Skip properties that aren't meant to be stored
  if (!(int(prop_info["usage"]) & PROPERTY_USAGE_STORAGE)) {
    return false;
  }

  // Skip certain types that shouldn't be serialized
  Variant::Type type = Variant::Type((int)prop_info["type"]);
  switch (type) {
    case Variant::CALLABLE:
    case Variant::SIGNAL:
    case Variant::RID:
      return false;
    default:
      return true;
  }
}

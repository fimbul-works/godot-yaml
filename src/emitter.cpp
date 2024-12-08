#include "emitter.h"
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

thread_local YAMLEmitter::EmitterInstance YAMLEmitter::t_emitter_instance;

YAMLEmitter::EmitterInstance::EmitterInstance()
{
  // Set up ryml error handling
  m_callbacks.m_error = error_callback;
  m_callbacks.m_user_data = this;
  m_evt_handler = std::make_unique<ryml::EventHandlerTree>(m_callbacks);
}

void YAMLEmitter::EmitterInstance::error_callback(const char* msg, size_t len, ryml::Location loc, void* user_data)
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

  // Set error in instance
  auto* instance = static_cast<EmitterInstance*>(user_data);
  if (!instance) {
    throw YAMLException(String::utf8(error_msg.str, error_msg.len));
  }

  instance->current_result = YAMLResult::error(
          String::utf8(error_msg.str, error_msg.len),
          loc.line,
          loc.col);

  throw YAMLException(instance->current_result->get_error());
}

Ref<YAMLResult> YAMLEmitter::emit(const Variant& input, const Ref<YAMLStyle>& style)
{
  try {
    auto& instance = t_emitter_instance;
    instance.current_style = style;
    instance.current_result = YAMLResult::success(Variant());
    instance.m_tree.clear();

    // Emit value into tree
    instance.emit_value(instance.m_tree.rootref(), input, style);

    // Check for errors that occurred during emission
    if (instance.current_result->has_error()) {
      return instance.current_result;
    }

    // Convert to YAML string
    std::string yaml_str = ryml::emitrs_yaml<std::string>(instance.m_tree);
    return YAMLResult::success(String::utf8(yaml_str.c_str(), yaml_str.length()), style);

  } catch (const YAMLException& e) {
    return YAMLResult::error(e.get_godot_message());
  } catch (const std::exception& e) {
    return YAMLResult::error(e.what());
  } catch (...) {
    return YAMLResult::error("Unknown error occurred during emission");
  }
}

void YAMLEmitter::EmitterInstance::emit_value(ryml::NodeRef& node, const Variant& value, const Ref<YAMLStyle>& style)
{
  if (style.is_valid() && !style->custom_settings.is_empty()) {
    if (style->custom_settings.has("tag")) {
      String tag = style->custom_settings["tag"];
      node.set_val_tag(to_ryml_str("!" + tag));
    }
  }

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
      if ((godot::Object*)obj) {
        emit_object(node, obj, style);
      } else {
        emit_nil(node);
      }
      break;
    }

    default:
      if (VariantConverterRegistry::has_converter(value.get_type())) {
        const auto* converter = VariantConverterRegistry::get_converter(value.get_type());
        node.set_val_tag(to_ryml_str(converter->get_full_tag()));
        converter->encode(node, value, style);
      } else {
        String type_name = Variant::get_type_name(value.get_type());
        current_result = YAMLResult::error(vformat("Unsupported type: %s", type_name));
      }
      break;
  }
}

void YAMLEmitter::EmitterInstance::emit_nil(ryml::NodeRef& node)
{
  ryml::csubstr null = {};
  node << null;
}

void YAMLEmitter::EmitterInstance::emit_bool(ryml::NodeRef& node, bool value)
{
  node << (value ? "true" : "false");
}

void YAMLEmitter::EmitterInstance::emit_number(ryml::NodeRef& node, const Variant& value, const Ref<YAMLStyle>& style)
{
  YAMLStyle::NumberFormat format = style.is_valid() ? style->number_format : YAMLStyle::NUM_DECIMAL;

  if (value.get_type() == Variant::INT) {
    int64_t int_val = static_cast<int64_t>(value.operator int64_t());
    node << int_to_string(int_val, format);
  } else {
    double float_val = static_cast<double>(value.operator double());
    node << float_to_string(float_val, format);
  }
}

void YAMLEmitter::EmitterInstance::emit_string(ryml::NodeRef& node, const String& value, const Ref<YAMLStyle>& style)
{
  if (value.is_empty()) {
    node << ryml::csubstr {};
    return;
  }

  // First handle explicit style settings if provided
  if (style.is_valid()) {
    switch (style->scalar_style) {
      case YAMLStyle::STYLE_BLOCK: {
        node |= ryml::BLOCK;

        // Set block style (literal or folded)
        if (style->block_style == YAMLStyle::BLOCK_LITERAL) {
          node |= ryml::VAL_LITERAL;
        } else if (style->block_style == YAMLStyle::BLOCK_FOLDED) {
          node |= ryml::VAL_FOLDED;
        }
        break;
      }
      case YAMLStyle::STYLE_QUOTED:
        node |= ryml::VAL_DQUO;
        break;
      case YAMLStyle::STYLE_PLAIN:
        // Only use block if needed
        if (needs_block_style(value)) {
          node |= ryml::BLOCK;
        }
        break;
      case YAMLStyle::STYLE_ANY:
        // Auto-detect appropriate style
        if (value.contains("\n")) {
          node |= ryml::BLOCK;
          // If the string has significant whitespace or ends with newlines,
          // use literal style, otherwise use folded
          if (value.ends_with("\n") || value.contains("  ")) {
            node |= ryml::VAL_LITERAL;
          } else {
            node |= ryml::VAL_FOLDED;
          }
        } else if (needs_block_style(value)) {
          node |= ryml::BLOCK;
        }
        break;
    }
  } else {
    // No style provided - use auto-detection
    if (value.contains("\n")) {
      node |= ryml::BLOCK;
      // If the string has significant whitespace or ends with newlines,
      // use literal style, otherwise use folded
      if (value.ends_with("\n") || value.contains("  ")) {
        node |= ryml::VAL_LITERAL;
      } else {
        node |= ryml::VAL_FOLDED;
      }
    } else if (needs_block_style(value)) {
      node |= ryml::BLOCK;
    }
  }

  node << to_ryml_str(value);
}

void YAMLEmitter::EmitterInstance::emit_array(ryml::NodeRef& node, const Array& array, const Ref<YAMLStyle>& style)
{
  node |= ryml::SEQ;

  if (array.size() == 0) {
    return;
  }

  if (style.is_valid() && style->collection_style == YAMLStyle::COLLECTION_FLOW) {
    node |= ryml::FLOW_SL;
  }

  // Get shared item style if available
  Ref<YAMLStyle> shared_style = style.is_valid() ? style->get_child("_items") : Ref<YAMLStyle>();

  for (int i = 0; i < array.size(); i++) {
    // Check for individual item style, fall back to shared style
    Ref<YAMLStyle> item_style;
    if (style.is_valid()) {
      item_style = style->get_child(String::num_int64(i));
      if (!item_style.is_valid()) {
        item_style = shared_style;
      }
    }
    emit_value(node.append_child(), array[i], item_style);
  }
}

void YAMLEmitter::EmitterInstance::emit_dictionary(ryml::NodeRef& node, const Dictionary& dict, const Ref<YAMLStyle>& style)
{
  node |= ryml::MAP;

  if (dict.size() == 0) {
    return;
  }

  if (style.is_valid() && style->collection_style == YAMLStyle::MAP_FLOW) {
    node |= ryml::FLOW_SL;
  }

  Array keys = dict.keys();
  for (int i = 0; i < keys.size(); i++) {
    const Variant& key = keys[i];
    ryml::csubstr key_str;

    // Convert key to string efficiently
    if (key.get_type() == Variant::STRING) {
      key_str = to_ryml_str(key);
    } else {
      key_str = to_ryml_str(String(key));
    }

    ryml::NodeRef child = node.append_child();
    child << ryml::key(key_str);

    // Pass child style based on key
    Ref<YAMLStyle> value_style = style.is_valid() ? style->get_child(String(key)) : Ref<YAMLStyle>();
    emit_value(child, dict[key], value_style);
  }
}

void YAMLEmitter::EmitterInstance::emit_object(ryml::NodeRef& node, const Object* obj, const Ref<YAMLStyle>& style)
{
  // Handle empty references
  if (!obj || obj == nullptr) {
    emit_nil(node);
    return;
  }

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

void YAMLEmitter::EmitterInstance::emit_resource(ryml::NodeRef& node, const Resource* res, const Ref<YAMLStyle>& style)
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

void YAMLEmitter::EmitterInstance::emit_object_properties(ryml::NodeRef& node, const Object* obj, const Ref<YAMLStyle>& style)
{
  Dictionary properties = ObjectReflection::get_object_properties(obj);
  Array prop_names = properties.keys();

  for (int i = 0; i < prop_names.size(); i++) {
    String prop_name = prop_names[i];
    Dictionary prop_info = properties[prop_name];

    if (should_serialize_property(prop_info)) {
      Variant value = obj->get(prop_name);
      emit_property_value(node, prop_name, value,
              style.is_valid() ? style->get_child(prop_name) : Ref<YAMLStyle>());
    }
  }
}

void YAMLEmitter::EmitterInstance::emit_property_value(ryml::NodeRef& node, const String& prop_name,
        const Variant& value, const Ref<YAMLStyle>& style)
{
  ryml::NodeRef child = node.append_child();
  child << ryml::key(to_ryml_str(prop_name));
  emit_value(child, value, style);
}

bool YAMLEmitter::EmitterInstance::should_serialize_property(const Dictionary& prop_info) const
{
  if (!prop_info.has("value")) {
    return false;
  }

  // Sometimes we deal with empty objects
  if (!prop_info.has("type") || !prop_info.has("usage")) {
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

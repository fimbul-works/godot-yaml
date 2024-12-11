#include "object_yaml.h"
#include "../exception.h"
#include "../reflection.h"

#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <memory>

using namespace godot;

void ObjectVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const YAMLStyle::View& style) const
{
  Object* obj = v.operator Object*();
  if (!obj) {
    UtilityFunctions::push_warning("YAML: Encoding ", v, " into Object has failed");
    node << ryml::csubstr {};
  }

  // Set class tag
  String class_name = obj->get_class();
  node.set_val_tag(store_string(class_name.utf8().get_data()));

  // Handle resources specially
  const Resource* res = Object::cast_to<const Resource>(obj);
  if (res) {
    emit_resource(node, res, style);
    return;
  }

  // For other objects, emit all properties
  node |= ryml::MAP;

  UtilityFunctions::print("Emitting Object of type ", class_name);

  emit_object_properties(node, obj, style);
}

void ObjectVariantConverter::emit_resource(ryml::NodeRef& node, const Resource* res, const YAMLStyle::View& style) const
{
  String path = res->get_path();

  // Check if resource has a path and no local modifications
  if (!path.is_empty() && ProjectSettings::get_singleton()->localize_path(path) == path) {
    // No local modifications - just emit the path
    node << ryml::VAL_DQUO;
    node << to_ryml_str(path);
    return;
  }

  // Resource has local modifications - emit as map with path and modified properties
  node |= ryml::MAP;

  // Store path if exists
  if (!path.is_empty()) {
    ryml::NodeRef path_node = node.append_child();
    path_node << ryml::key(to_ryml_str("path"));
    path_node |= ryml::VAL_DQUO;
    path_node << to_ryml_str(path);
  }

  emit_object_properties(node, res, style);
}

void ObjectVariantConverter::emit_object_properties(ryml::NodeRef& node, const Object* obj, const YAMLStyle::View& style) const
{
  String class_name = obj->get_class();
  UtilityFunctions::print("Encoding properties of ", class_name);

  Dictionary properties = ObjectReflection::get_object_properties(obj);
  Array prop_names = properties.keys();

  for (int i = 0; i < prop_names.size(); i++) {
    String prop_name = prop_names[i];
    Dictionary prop_info = properties[prop_name];

    if (should_serialize_property(prop_info)) {
      Variant value = obj->get(prop_name);
      UtilityFunctions::print("  - ", prop_name, ": ", value);
      emit_property_value(node, prop_name, value,
              style.is_valid() ? style.get_child(prop_name) : YAMLStyle::View());
    }
  }
}

void ObjectVariantConverter::emit_property_value(ryml::NodeRef& node, const String& prop_name, const Variant& value, const YAMLStyle::View& style) const
{
  ryml::NodeRef child = node.append_child();
  child << ryml::key(to_ryml_str(prop_name));
  child << "TODO: emit property value";
  //emit_value(child, value, style);
}

bool ObjectVariantConverter::should_serialize_property(const Dictionary& prop_info) const
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

Variant ObjectVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  // Determine class name
  String class_name = "Object";
  if (node.has_val_tag()) {
    const auto& tag = node.val_tag();
    class_name = tag.begins_with('!') ? String::utf8(tag.sub(1).str, tag.len - 1) : from_ryml_str(tag);
  }

  try {
    if (node.val_is_null()) {
      return Variant();
    }

    if (!node.has_val()) {
      throw YAMLException::create_invalid_format(class_name.utf8().get_data());
    }

    return Variant();
  } catch (const YAMLException&) {
    throw;
  } catch (const std::exception& e) {
    throw YAMLException::create_decode_error(class_name.utf8().get_data(), e.what());
  }
}

#include "resource_yaml.h"
#include "../yaml_exception.h"
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/resource_loader.hpp>

using namespace godot;

ResourceVariantConverter::ResourceVariantConverter(YAML* yaml) :
        VariantConverter(yaml) { }

void ResourceVariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  Object* obj = v.operator Object*();
  Resource* res = Object::cast_to<Resource>(obj);

  if (!res) {
    throw YAMLException("Only Resource objects are currently supported");
  }

  String path = res->get_path();
  if (path.is_empty()) {
    throw YAMLException("Resource must have a valid path");
  }

  node << path.utf8().get_data();
}

Variant ResourceVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (!node.has_val() || node.val_is_null()) {
    throw YAMLException::create_invalid_format("Resource");
  }

  String path = String::utf8(node.val().str, node.val().len);
  if (!validate_path(path)) {
    throw YAMLException(String("Invalid resource path: ") + path);
  }

  Ref<Resource> resource = ResourceLoader::get_singleton()->load(path);
  if (resource.is_null()) {
    throw YAMLException(String("Failed to load resource from path: ") + path);
  }

  return resource;
}

bool ResourceVariantConverter::set_format(const String& format_str)
{
  // Resource only supports path format
  return true;
}

bool ResourceVariantConverter::validate_path(const String& path) const
{
  // Basic path validation
  if (path.is_empty()) {
    return false;
  }

  // Ensure path is local (basic security check)
  String local_path = ProjectSettings::get_singleton()->localize_path(path);
  if (path != local_path) {
    return false;
  }

  // You might want to add more validation here later
  return true;
}

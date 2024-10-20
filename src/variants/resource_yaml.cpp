#include "resource_yaml.h"
#include "yaml.h"

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

ResourceYAMLEncoder::ResourceYAMLEncoder(YAML* yaml)
{
  m_yaml = yaml;
}

void ResourceYAMLEncoder::encode(ryml::NodeRef& node, const Ref<Resource>& r) const
{
  if (r.is_valid()) {
    node << r->get_path().utf8().get_data();
  }
}

Variant ResourceYAMLEncoder::decode(const ryml::ConstNodeRef& node) const
{
  if (node.has_val() && !node.val_is_null()) {
    String path = String::utf8(node.val().str, node.val().len);
    Ref<Resource> loaded_resource = ResourceLoader::get_singleton()->load(path);
    if (loaded_resource.is_null()) {
      throw YAMLException("Failed to load resource from path: " + path);
    }
    return loaded_resource;
  }
  UtilityFunctions::print("has val: ", node.has_val(), " is null: ", node.val_is_null());
  throw YAMLException("invalid Resource format - " + String::utf8(node.val().str, node.val().len));
}

bool ResourceYAMLEncoder::set_format(const String& format_str)
{
  return true;
}

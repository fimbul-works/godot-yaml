#include "node_path_yaml.h"
#include "yaml.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

NodePathVariantConverter::NodePathVariantConverter(YAML* yaml) :
        VariantConverter(yaml) { }

void NodePathVariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  NodePath node_path = v.operator NodePath();
  node << String(node_path.get_concatenated_names()).utf8().get_data();
}

Variant NodePathVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (node.has_val() && !node.val_is_null()) {
    return NodePath(String::utf8(node.val().str, node.val().len));
  }
  throw YAMLException("invalid NodePath format - " + String::utf8(node.val().str, node.val().len));
}

bool NodePathVariantConverter::set_format(const String& format_str)
{
  return true;
}

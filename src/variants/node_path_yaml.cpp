#include "node_path_yaml.h"
#include "../yaml_exception.h"

using namespace godot;

NodePathVariantConverter::NodePathVariantConverter(YAML* yaml) :
        VariantConverter(yaml) { }

void NodePathVariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  NodePath node_path = v.operator NodePath();
  String path = String(node_path.get_concatenated_names());
  if (path.is_empty()) {
    // Empty node path is represented as null
    ryml::csubstr null = {};
    node << null;
  } else {
    node << path.utf8().get_data();
  }
}

Variant NodePathVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (node.val_is_null()) {
    return NodePath(); // Return empty NodePath
  }

  if (!node.has_val()) {
    throw YAMLException::create_invalid_format("NodePath");
  }

  String path = String::utf8(node.val().str, node.val().len);
  try {
    return NodePath(path);
  } catch (const std::exception& e) {
    throw YAMLException(String("Failed to create NodePath: ") + e.what());
  }
}

bool NodePathVariantConverter::set_format(const String& format)
{
  // NodePath only supports a single format (string)
  return true;
}

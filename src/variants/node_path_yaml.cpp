#include "node_path_yaml.h"
#include "../exception.h"

using namespace godot;

void NodePathVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const Ref<YAMLStyle>& style) const
{
  const NodePath path = v.operator NodePath();
  emit_as_string(node, path);
}

void NodePathVariantConverter::emit_as_string(ryml::NodeRef& node, const NodePath& path) const
{
  String str = String(path);
  if (str.is_empty()) {
    // Empty node path is represented as null
    ryml::csubstr null = {};
    node << null;
  } else {
    node << str.utf8().get_data();
  }
}

Variant NodePathVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  try {
    if (node.val_is_null()) {
      return NodePath(); // Return empty NodePath
    }

    if (!node.has_val()) {
      throw YAMLException::create_invalid_format("NodePath");
    }

    return decode_from_string(node.val());
  } catch (const YAMLException&) {
    throw;
  } catch (const std::exception& e) {
    throw YAMLException::create_decode_error("NodePath", e.what());
  }
}

Variant NodePathVariantConverter::decode_from_string(const ryml::csubstr& val) const
{
  try {
    String path_str = String::utf8(val.str, val.len);
    return NodePath(path_str);
  } catch (const std::exception& e) {
    throw YAMLException(String("Invalid NodePath format: ") + e.what());
  }
}

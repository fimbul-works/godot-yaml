#ifndef NODE_PATH_YAML_H
#define NODE_PATH_YAML_H

#include "../variant_converter.h"

namespace godot {

/**
 * YAML converter for NodePath type.
 * Represents node paths as strings in YAML, with null/empty scalar for empty paths.
 * Examples:
 * - "Path/To/Node"
 * - "Path/To/Node:property"
 * - null (for empty path)
 */
class NodePathVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("NodePath", Variant::NODE_PATH)

  void encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  private:
  void emit_as_string(ryml::NodeRef& node, const NodePath& path) const;
  Variant decode_from_string(const ryml::csubstr& val) const;
};

} // namespace godot

#endif // NODE_PATH_YAML_H

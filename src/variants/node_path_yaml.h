#ifndef NODE_PATH_YAML_H
#define NODE_PATH_YAML_H

#include "../variant_converter.h"

namespace godot {

class NodePathVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("NodePath", Variant::NODE_PATH)

  void encode(ryml::NodeRef& node, const Variant& v, const YAMLStyle::View& style) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  private:
  void emit_as_string(ryml::NodeRef& node, const NodePath& path) const;
  Variant decode_from_string(const ryml::csubstr& val) const;
};

} // namespace godot

#endif // NODE_PATH_YAML_H

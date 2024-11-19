#ifndef NODE_PATH_YAML_H
#define NODE_PATH_YAML_H

#include "../variant_converter.h"

namespace godot {

class NodePathVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("NodePath", Variant::NODE_PATH)

  NodePathVariantConverter(YAML* yaml);

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;
  bool set_format(const String& format) override;
};

} // namespace godot

#endif // NODE_PATH_YAML_H

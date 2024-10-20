#ifndef NODE_PATH_YAML_H
#define NODE_PATH_YAML_H

#include "yaml_encoder.h"

namespace godot {

class NodePathYAMLEncoder : public YAMLEncoder {
  public:
  DEFINE_YAML_TAG("NodePath", Variant::NODE_PATH)

  NodePathYAMLEncoder(YAML* yaml);

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  bool set_format(const String& format_str) override;
};

} // namespace godot

#endif // NODE_PATH_YAML_H

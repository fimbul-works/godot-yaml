#ifndef TRANSFORM2D_YAML_H
#define TRANSFORM2D_YAML_H

#include "vector2_yaml.h"
#include "yaml.h"

namespace godot {

class Transform2DYAMLEncoder : public YAMLEncoder {
  public:
  DEFINE_YAML_TAG("Transform2D", Variant::TRANSFORM2D)

  Transform2DYAMLEncoder(YAML* yaml);
  ~Transform2DYAMLEncoder();

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  bool set_format(const String& format_str) override;

  private:
  void emit_as_map(ryml::NodeRef& node, const Transform2D& transform) const;

  Vector2YAMLEncoder* vec_encoder;
};

} // namespace godot

#endif // TRANFORM2D_YAML_H

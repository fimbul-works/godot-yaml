#ifndef TRANSFORM3D_YAML_H
#define TRANSFORM3D_YAML_H

#include "vector3_yaml.h"
#include "yaml.h"

namespace godot {

class Transform3DYAMLEncoder : public YAMLEncoder {
  public:
  DEFINE_YAML_TAG("Transform3D", Variant::TRANSFORM3D)

  Transform3DYAMLEncoder(YAML* yaml);
  ~Transform3DYAMLEncoder();

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  bool set_format(const String& format_str) override;

  private:
  void emit_as_map(ryml::NodeRef& node, const Transform3D& transform) const;

  Vector3YAMLEncoder* vec_encoder;
};

} // namespace godot

#endif // TRANSFORM3D_YAML_H

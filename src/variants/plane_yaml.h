#ifndef PLANE_YAML_H
#define PLANE_YAML_H

#include "vector3_yaml.h"
#include "yaml.h"

namespace godot {

class PlaneYAMLEncoder : public YAMLEncoder {
  public:
  DEFINE_YAML_TAG("Plane")

  PlaneYAMLEncoder(YAML* yaml);
  ~PlaneYAMLEncoder();

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  bool set_format(const String& format_str) override;

  private:
  void emit_as_map(ryml::NodeRef& node, const Plane& aabb) const;

  Vector3YAMLEncoder* vec_encoder;
};

} // namespace godot

#endif // PLANE_YAML_H

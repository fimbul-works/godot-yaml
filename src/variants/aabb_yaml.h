#ifndef AABB_YAML_H
#define AABB_YAML_H

#include "vector3_yaml.h"
#include "yaml.h"
#include "yaml_encoder.h"

namespace godot {

class AABBYAMLEncoder : public YAMLEncoder {
  public:
  DEFINE_YAML_TAG("AABB", Variant::AABB)

  AABBYAMLEncoder(YAML* yaml);
  ~AABBYAMLEncoder();

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  bool set_format(const String& format_str) override;

  private:
  void emit_as_map(ryml::NodeRef& node, const AABB& aabb) const;

  Vector3YAMLEncoder* vec_encoder;
};

} // namespace godot

#endif // AABB_YAML_H

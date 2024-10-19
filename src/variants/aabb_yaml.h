#ifndef AABB_YAML_H
#define AABB_YAML_H

#include "vector3_yaml.h"
#include "yaml.h"
#include "yaml_encoder.h"
#include <godot_cpp/variant/aabb.hpp>

namespace godot {

class AABBYAMLEncoder : public IYAMLEncoder {
  public:
  AABBYAMLEncoder();

  const char* get_tag() const override;

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  bool set_format(const String& format_str) override;

  private:
  void emit_as_map(ryml::NodeRef& node, const AABB& aabb) const;

  Vector3YAMLEncoder vec3_encoder;
};

} // namespace godot

#endif // AABB_YAML_H

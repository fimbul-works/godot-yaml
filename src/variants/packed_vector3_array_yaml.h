#ifndef PACKED_VECTOR3_ARRAY_YAML_H
#define PACKED_VECTOR3_ARRAY_YAML_H

#include "vector3_yaml.h"
#include "yaml_encoder.h"

namespace godot {

class PackedVector3ArrayYAMLEncoder : public IYAMLEncoder {
  public:
  const char* get_tag() const override;

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  bool set_format(const String& format_str) override;

  Vector3YAMLEncoder vec_encoder;
};

} // namespace godot

#endif // PACKED_VECTOR3_ARRAY_YAML_H

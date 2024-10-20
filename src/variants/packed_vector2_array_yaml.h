#ifndef PACKED_VECTOR2_ARRAY_YAML_H
#define PACKED_VECTOR2_ARRAY_YAML_H

#include "vector2_yaml.h"
#include "yaml_encoder.h"

namespace godot {

class PackedVector2ArrayYAMLEncoder : public YAMLEncoder {
  public:
  DEFINE_YAML_TAG("PackedVector2Array", Variant::PACKED_VECTOR2_ARRAY)

  PackedVector2ArrayYAMLEncoder(YAML* yaml);
  ~PackedVector2ArrayYAMLEncoder();

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  bool set_format(const String& format_str) override;

  Vector2YAMLEncoder* vec_encoder;
};

} // namespace godot

#endif // PACKED_VECTOR2_ARRAY_YAML_H

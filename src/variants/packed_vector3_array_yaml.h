#ifndef PACKED_VECTOR3_ARRAY_YAML_H
#define PACKED_VECTOR3_ARRAY_YAML_H

#include "../variant_converter.h"
#include "vector3_yaml.h"

namespace godot {

class PackedVector3ArrayVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("PackedVector3Array", Variant::PACKED_VECTOR3_ARRAY)

  PackedVector3ArrayVariantConverter(YAML* yaml);
  ~PackedVector3ArrayVariantConverter();

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;
  bool set_format(const String& format) override;

  private:
  Vector3VariantConverter* vec_encoder;
};

} // namespace godot

#endif // PACKED_VECTOR3_ARRAY_YAML_H

#ifndef PACKED_VECTOR2_ARRAY_YAML_H
#define PACKED_VECTOR2_ARRAY_YAML_H

#include "../variant_converter.h"
#include "vector2_yaml.h"

namespace godot {

class PackedVector2ArrayVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("PackedVector2Array", Variant::PACKED_VECTOR2_ARRAY)

  PackedVector2ArrayVariantConverter(YAML* yaml);
  ~PackedVector2ArrayVariantConverter();

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;
  bool set_format(const String& format) override;

  private:
  Vector2VariantConverter* vec_encoder;
};

} // namespace godot

#endif // PACKED_VECTOR2_ARRAY_YAML_H

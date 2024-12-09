#ifndef PACKED_VECTOR2_ARRAY_YAML_H
#define PACKED_VECTOR2_ARRAY_YAML_H

#include "../variant_converter.h"

namespace godot {

class PackedVector2ArrayVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("PackedVector2Array", Variant::PACKED_VECTOR2_ARRAY)

  void encode(ryml::NodeRef& node, const Variant& v, const YAMLStyle::View& style) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;
};

} // namespace godot

#endif // PACKED_VECTOR2_ARRAY_YAML_H

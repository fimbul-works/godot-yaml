#ifndef PACKED_VECTOR2_ARRAY_YAML_H
#define PACKED_VECTOR2_ARRAY_YAML_H

#include "../variant_converter.h"

namespace godot {

/**
 * YAML converter for PackedVector2Array type.
 * Vector2 elements in the array use the same formats as individual Vector2 values.
 * Format for vector components is controlled by the YAMLFormat settings.
 * Empty arrays are represented as empty sequences.
 * Example: [{x: 0, y: 0}, {x: 1, y: 2}]
 */
class PackedVector2ArrayVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("PackedVector2Array", Variant::PACKED_VECTOR2_ARRAY)

  void encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;
};

} // namespace godot

#endif // PACKED_VECTOR2_ARRAY_YAML_H

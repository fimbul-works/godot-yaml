#ifndef PACKED_FLOAT64_ARRAY_YAML_H
#define PACKED_FLOAT64_ARRAY_YAML_H

#include "../variant_converter.h"

namespace godot {

/**
 * YAML converter for PackedFloat64Array type.
 * Represents arrays of 64-bit floating point numbers as a sequence.
 * Handles special values like infinity and NaN.
 * Empty arrays are represented as empty sequences.
 * Example: [1.0, -2.5, .inf, -.inf, .nan]
 */
class PackedFloat64ArrayVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("PackedFloat64Array", Variant::PACKED_FLOAT64_ARRAY)

  void encode(ryml::NodeRef& node, const Variant& v, const Ref<YAMLStyle>& style) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  private:
  void emit_as_sequence(ryml::NodeRef& node, const PackedFloat64Array& array, const Ref<YAMLStyle>& style) const;
};

} // namespace godot

#endif // PACKED_FLOAT64_ARRAY_YAML_H

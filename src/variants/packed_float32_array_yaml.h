#ifndef PACKED_FLOAT32_ARRAY_YAML_H
#define PACKED_FLOAT32_ARRAY_YAML_H

#include "../variant_converter.h"

namespace godot {

/**
 * YAML converter for PackedFloat32Array type.
 * Represents arrays of 32-bit floating point numbers as a sequence.
 * Handles special values like infinity and NaN.
 * Empty arrays are represented as empty sequences.
 * Example: [1.0, -2.5, .inf, -.inf, .nan]
 */
class PackedFloat32ArrayVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("PackedFloat32Array", Variant::PACKED_FLOAT32_ARRAY)

  void encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  private:
  void emit_as_sequence(ryml::NodeRef& node, const PackedFloat32Array& array, const YAMLFormat::View& format) const;
};

} // namespace godot

#endif // PACKED_FLOAT32_ARRAY_YAML_H

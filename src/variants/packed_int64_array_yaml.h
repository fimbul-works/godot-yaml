#ifndef PACKED_INT64_ARRAY_YAML_H
#define PACKED_INT64_ARRAY_YAML_H

#include "../variant_converter.h"

namespace godot {

/**
 * YAML converter for PackedInt64Array type.
 * Represents arrays of 64-bit integers as a sequence.
 * Supports decimal, hexadecimal, octal, and binary formats for individual numbers.
 * Empty arrays are represented as empty sequences.
 * Examples:
 * - [1, 2, 3]
 * - [0xFFFFFFFF, 0b1010, 9223372036854775807]
 */
class PackedInt64ArrayVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("PackedInt64Array", Variant::PACKED_INT64_ARRAY)

  void encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  private:
  void emit_as_sequence(ryml::NodeRef& node, const PackedInt64Array& array, const YAMLFormat::View& format) const;
};

} // namespace godot

#endif // PACKED_INT64_ARRAY_YAML_H

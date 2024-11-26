#ifndef PACKED_STRING_ARRAY_YAML_H
#define PACKED_STRING_ARRAY_YAML_H

#include "../variant_converter.h"

namespace godot {

/**
 * YAML converter for PackedStringArray type.
 * Handles arrays of strings, supporting:
 * - Empty strings
 * - Multi-line strings
 * - Strings with special characters
 * Empty arrays are represented as empty sequences.
 * Examples:
 * - ["a", "b", "c"]
 * - ["", "non-empty", "multi\nline"]
 */
class PackedStringArrayVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("PackedStringArray", Variant::PACKED_STRING_ARRAY)

  void encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  private:
  void emit_as_sequence(ryml::NodeRef& node, const PackedStringArray& array, const YAMLFormat::View& format) const;
};

} // namespace godot

#endif // PACKED_STRING_ARRAY_YAML_H

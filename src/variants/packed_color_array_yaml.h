#ifndef PACKED_COLOR_ARRAY_YAML_H
#define PACKED_COLOR_ARRAY_H

#include "../variant_converter.h"

namespace godot {

/**
 * YAML converter for PackedColorArray type.
 * Colors in the array are encoded using the same formats as individual Color values.
 * Format is controlled by the YAMLFormat settings.
 * Empty arrays are represented as empty sequences.
 */
class PackedColorArrayVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("PackedColorArray", Variant::PACKED_COLOR_ARRAY)

  void encode(ryml::NodeRef& node, const Variant& v, const YAMLStyle::View& style) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;
};

} // namespace godot

#endif // PACKED_COLOR_ARRAY_YAML_H

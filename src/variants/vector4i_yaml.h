#ifndef VECTOR4I_YAML_H
#define VECTOR4I_YAML_H

#include "../variant_converter.h"

namespace godot {

/**
 * YAML converter for Vector4i type.
 * Supports the following formats:
 * - Map: {x: 0, y: 0, z: 0, w: 0}
 * - Sequence: [0, 0, 0, 0]
 */
class Vector4iVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("Vector4i", Variant::VECTOR4I)

  void encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  private:
  void emit_as_map(ryml::NodeRef& node, const Vector4i& vec, const YAMLFormat::View& format) const;
  void emit_as_sequence(ryml::NodeRef& node, const Vector4i& vec, const YAMLFormat::View& format) const;

  Variant decode_from_map(const ryml::ConstNodeRef& node) const;
  Variant decode_from_sequence(const ryml::ConstNodeRef& node) const;
};

} // namespace godot

#endif // VECTOR4I_YAML_H

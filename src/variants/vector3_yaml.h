#ifndef VECTOR3_YAML_H
#define VECTOR3_YAML_H

#include "../variant_converter.h"

namespace godot {

/**
 * YAML converter for Vector3 type.
 * Supports the following formats:
 * - Map: {x: 0.0, y: 0.0, z: 0.0}
 * - Sequence: [0.0, 0.0, 0.0]
 */
class Vector3VariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("Vector3", Variant::VECTOR3)

  void encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  private:
  void emit_as_map(ryml::NodeRef& node, const Vector3& vec) const;
  void emit_as_sequence(ryml::NodeRef& node, const Vector3& vec) const;

  Variant decode_from_map(const ryml::ConstNodeRef& node) const;
  Variant decode_from_sequence(const ryml::ConstNodeRef& node) const;
};

} // namespace godot

#endif // VECTOR3_YAML_H

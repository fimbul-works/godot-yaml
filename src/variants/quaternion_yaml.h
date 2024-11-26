#ifndef QUATERNION_YAML_H
#define QUATERNION_YAML_H

#include "../variant_converter.h"

namespace godot {

/**
 * YAML converter for Quaternion type.
 * Supports the following formats:
 * - Map: {x: 0, y: 0, z: 0, w: 1}
 * - Sequence: [x, y, z, w]
 * - Axis-Angle map: {axis: {x: 1, y: 0, z: 0}, angle: 1.5708}
 */
class QuaternionVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("Quaternion", Variant::QUATERNION)

  void encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  private:
  void emit_as_map(ryml::NodeRef& node, const Quaternion& quat, const YAMLFormat::View& format) const;
  void emit_as_sequence(ryml::NodeRef& node, const Quaternion& quat, const YAMLFormat::View& format) const;
  void emit_as_axis_angle(ryml::NodeRef& node, const Quaternion& quat, const YAMLFormat::View& format) const;

  Variant decode_from_map(const ryml::ConstNodeRef& node) const;
  Variant decode_from_sequence(const ryml::ConstNodeRef& node) const;
  Variant decode_from_axis_angle(const ryml::ConstNodeRef& node) const;

  const VariantConverter* get_vec3_converter() const;
};

} // namespace godot

#endif // QUATERNION_YAML_H

#ifndef PLANE_YAML_H
#define PLANE_YAML_H

#include "../variant_converter.h"

namespace godot {

/**
 * YAML converter for Plane type.
 * Supports the following formats:
 * - Map: {normal: {x: 0, y: 1, z: 0}, d: 5.0}
 * - Sequence: [{x: 0, y: 1, z: 0}, 5.0]
 * The normal vector uses the same format as Vector3.
 */
class PlaneVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("Plane", Variant::PLANE)

  void encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  private:
  void emit_as_map(ryml::NodeRef& node, const Plane& plane, const YAMLFormat::View& format) const;
  void emit_as_sequence(ryml::NodeRef& node, const Plane& plane, const YAMLFormat::View& format) const;
  Variant decode_from_map(const ryml::ConstNodeRef& node) const;
  Variant decode_from_sequence(const ryml::ConstNodeRef& node) const;
  const VariantConverter* get_vec3_converter() const;
};

} // namespace godot

#endif // PLANE_YAML_H

#ifndef TRANSFORM3D_YAML_H
#define TRANSFORM3D_YAML_H

#include "../variant_converter.h"

namespace godot {

/**
 * YAML converter for Transform3D type.
 * Supports the following formats:
 * - Map: {basis: {x: {x,y,z}, y: {x,y,z}, z: {x,y,z}}, origin: {x,y,z}}
 * - Sequence: [{x,y,z}, {x,y,z}, {x,y,z}, {x,y,z}] (basis columns + origin)
 */
class Transform3DVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("Transform3D", Variant::TRANSFORM3D)

  void encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  private:
  void emit_as_map(ryml::NodeRef& node, const Transform3D& transform, const YAMLFormat::View& format) const;
  void emit_as_sequence(ryml::NodeRef& node, const Transform3D& transform, const YAMLFormat::View& format) const;

  Variant decode_from_map(const ryml::ConstNodeRef& node) const;
  Variant decode_from_sequence(const ryml::ConstNodeRef& node) const;

  const VariantConverter* get_vec3_converter() const;
  const VariantConverter* get_basis_converter() const;
};

} // namespace godot

#endif // TRANSFORM3D_YAML_H

#ifndef PROJECTION_YAML_H
#define PROJECTION_YAML_H

#include "../variant_converter.h"

namespace godot {

/**
 * YAML converter for Projection type.
 * Supports the following formats:
 * - Map with Vector4 columns: {x: {x,y,z,w}, y: {x,y,z,w}, z: {x,y,z,w}, w: {x,y,z,w}}
 * - Map with arrays: {x: [x,y,z,w], y: [x,y,z,w], z: [x,y,z,w], w: [x,y,z,w]}
 * - Sequence of Vector4: [{x,y,z,w}, {x,y,z,w}, {x,y,z,w}, {x,y,z,w}]
 * - Sequence of arrays: [[x,y,z,w], [x,y,z,w], [x,y,z,w], [x,y,z,w]]
 */
class ProjectionVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("Projection", Variant::PROJECTION)

  void encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  private:
  void emit_as_map(ryml::NodeRef& node, const Projection& proj, const YAMLFormat::View& format) const;
  void emit_as_sequence(ryml::NodeRef& node, const Projection& proj, const YAMLFormat::View& format) const;
  void emit_column(ryml::NodeRef& node, const Vector4& col, const YAMLFormat::View& format) const;

  Variant decode_from_map(const ryml::ConstNodeRef& node) const;
  Variant decode_from_sequence(const ryml::ConstNodeRef& node) const;
  Vector4 decode_column(const ryml::ConstNodeRef& node) const;
  Vector4 decode_array_column(const ryml::ConstNodeRef& node) const;
};

} // namespace godot

#endif // PROJECTION_YAML_H

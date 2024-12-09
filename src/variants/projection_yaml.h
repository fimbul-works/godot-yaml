#ifndef PROJECTION_YAML_H
#define PROJECTION_YAML_H

#include "../variant_converter.h"

namespace godot {

class ProjectionVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("Projection", Variant::PROJECTION)

  void encode(ryml::NodeRef& node, const Variant& v, const YAMLStyle::View& style) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  private:
  void emit_as_map(ryml::NodeRef& node, const Projection& proj, const YAMLStyle::View& style) const;
  void emit_as_sequence(ryml::NodeRef& node, const Projection& proj, const YAMLStyle::View& style) const;
  void emit_column(ryml::NodeRef& node, const Vector4& col, const YAMLStyle::View& style) const;

  Variant decode_from_map(const ryml::ConstNodeRef& node) const;
  Variant decode_from_sequence(const ryml::ConstNodeRef& node) const;
  Vector4 decode_column(const ryml::ConstNodeRef& node) const;
  Vector4 decode_array_column(const ryml::ConstNodeRef& node) const;
};

} // namespace godot

#endif // PROJECTION_YAML_H

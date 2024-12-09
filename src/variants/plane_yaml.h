#ifndef PLANE_YAML_H
#define PLANE_YAML_H

#include "../variant_converter.h"

namespace godot {

class PlaneVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("Plane", Variant::PLANE)

  void encode(ryml::NodeRef& node, const Variant& v, const YAMLStyle::View& style) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  private:
  void emit_as_map(ryml::NodeRef& node, const Plane& plane, const YAMLStyle::View& style) const;
  void emit_as_sequence(ryml::NodeRef& node, const Plane& plane, const YAMLStyle::View& style) const;
  Variant decode_from_map(const ryml::ConstNodeRef& node) const;
  Variant decode_from_sequence(const ryml::ConstNodeRef& node) const;
};

} // namespace godot

#endif // PLANE_YAML_H

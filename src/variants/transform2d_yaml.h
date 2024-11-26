#ifndef TRANSFORM2D_YAML_H
#define TRANSFORM2D_YAML_H

#include "../variant_converter.h"

namespace godot {

/**
 * YAML converter for Transform2D type.
 * Supports the following formats:
 * - Map: {x: {x: 1, y: 0}, y: {x: 0, y: 1}, origin: {x: 0, y: 0}}
 * - Sequence: [{x: 1, y: 0}, {x: 0, y: 1}, {x: 0, y: 0}]
 */
class Transform2DVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("Transform2D", Variant::TRANSFORM2D)

  void encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  private:
  void emit_as_map(ryml::NodeRef& node, const Transform2D& transform, const YAMLFormat::View& format) const;
  void emit_as_sequence(ryml::NodeRef& node, const Transform2D& transform, const YAMLFormat::View& format) const;

  Variant decode_from_map(const ryml::ConstNodeRef& node) const;
  Variant decode_from_sequence(const ryml::ConstNodeRef& node) const;

  const VariantConverter* get_vec2_converter() const;
};

} // namespace godot

#endif // TRANSFORM2D_YAML_H

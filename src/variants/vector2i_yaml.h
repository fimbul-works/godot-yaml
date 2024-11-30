#ifndef VECTOR2I_YAML_H
#define VECTOR2I_YAML_H

#include "../variant_converter.h"

namespace godot {

/**
 * YAML converter for Vector2i type.
 * Supports the following formats:
 * - Map: {x: 0, y: 0}
 * - Sequence: [0, 0]
 */
class Vector2iVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("Vector2i", Variant::VECTOR2I)

  void encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  private:
  void emit_as_map(ryml::NodeRef& node, const Vector2i& vec, const YAMLFormat::View& format) const;
  void emit_as_sequence(ryml::NodeRef& node, const Vector2i& vec, const YAMLFormat::View& format) const;

  Variant decode_from_map(const ryml::ConstNodeRef& node) const;
  Variant decode_from_sequence(const ryml::ConstNodeRef& node) const;
};

} // namespace godot

#endif // VECTOR2I_YAML_H

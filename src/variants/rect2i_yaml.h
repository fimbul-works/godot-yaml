#ifndef RECT2I_YAML_H
#define RECT2I_YAML_H

#include "../variant_converter.h"

namespace godot {

/**
 * YAML converter for Rect2i type.
 * Supports the following formats:
 * - Map: {position: {x: 0, y: 0}, size: {x: 100, y: 50}}
 * - Sequence: [{x: 0, y: 0}, {x: 100, y: 50}]
 * - Expanded map: {x: 0, y: 0, w: 100, h: 50}
 */
class Rect2iVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("Rect2i", Variant::RECT2I)

  void encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  private:
  void emit_as_map(ryml::NodeRef& node, const Rect2i& rect, const YAMLFormat::View& format) const;
  void emit_as_sequence(ryml::NodeRef& node, const Rect2i& rect, const YAMLFormat::View& format) const;
  void emit_as_expanded(ryml::NodeRef& node, const Rect2i& rect) const;

  Variant decode_from_map(const ryml::ConstNodeRef& node) const;
  Variant decode_from_sequence(const ryml::ConstNodeRef& node) const;
  Variant decode_from_expanded(const ryml::ConstNodeRef& node) const;
};

} // namespace godot

#endif // RECT2I_YAML_H

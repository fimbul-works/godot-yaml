#ifndef RECT2_YAML_H
#define RECT2_YAML_H

#include "../variant_converter.h"

namespace godot {

class Rect2VariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("Rect2", Variant::RECT2)

  void encode(ryml::NodeRef& node, const Variant& v, const YAMLStyle::View& style) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  private:
  void emit_as_map(ryml::NodeRef& node, const Rect2& rect, const YAMLStyle::View& style) const;
  void emit_as_sequence(ryml::NodeRef& node, const Rect2& rect, const YAMLStyle::View& style) const;

  Variant decode_from_map(const ryml::ConstNodeRef& node) const;
  Variant decode_from_sequence(const ryml::ConstNodeRef& node) const;
};

} // namespace godot

#endif // RECT2_YAML_H

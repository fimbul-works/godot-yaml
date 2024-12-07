#ifndef RECT2I_YAML_H
#define RECT2I_YAML_H

#include "../variant_converter.h"

namespace godot {

class Rect2iVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("Rect2i", Variant::RECT2I)

  void encode(ryml::NodeRef& node, const Variant& v, const Ref<YAMLStyle>& style) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  private:
  void emit_as_map(ryml::NodeRef& node, const Rect2i& rect, const Ref<YAMLStyle>& style) const;
  void emit_as_sequence(ryml::NodeRef& node, const Rect2i& rect, const Ref<YAMLStyle>& style) const;

  Variant decode_from_map(const ryml::ConstNodeRef& node) const;
  Variant decode_from_sequence(const ryml::ConstNodeRef& node) const;
};

} // namespace godot

#endif // RECT2I_YAML_H

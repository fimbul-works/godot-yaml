#ifndef VECTOR2I_YAML_H
#define VECTOR2I_YAML_H

#include "../variant_converter.h"

namespace godot {

class Vector2iVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("Vector2i", Variant::VECTOR2I)

  void encode(ryml::NodeRef& node, const Variant& v, const Ref<YAMLStyle>& style) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  private:
  Variant decode_from_map(const ryml::ConstNodeRef& node) const;
  Variant decode_from_sequence(const ryml::ConstNodeRef& node) const;
};

} // namespace godot

#endif // VECTOR2I_YAML_H

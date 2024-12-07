#ifndef PACKED_VECTOR3_ARRAY_YAML_H
#define PACKED_VECTOR3_ARRAY_YAML_H

#include "../variant_converter.h"

namespace godot {

class PackedVector3ArrayVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("PackedVector3Array", Variant::PACKED_VECTOR3_ARRAY)

  void encode(ryml::NodeRef& node, const Variant& v, const Ref<YAMLStyle>& style) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;
};

} // namespace godot

#endif // PACKED_VECTOR3_ARRAY_YAML_H

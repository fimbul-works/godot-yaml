#ifndef VECTOR3I_YAML_H
#define VECTOR3I_YAML_H

#include "../variant_converter.h"

namespace godot {

class Vector3iVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("Vector3i", Variant::VECTOR3I)

  void encode(ryml::NodeRef& node, const Variant& v, const YAMLStyle::View& style) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  private:
  Variant decode_from_map(const ryml::ConstNodeRef& node) const;
  Variant decode_from_sequence(const ryml::ConstNodeRef& node) const;
};

} // namespace godot

#endif // VECTOR3I_YAML_H

#ifndef PACKED_FLOAT32_ARRAY_YAML_H
#define PACKED_FLOAT32_ARRAY_YAML_H

#include "../variant_converter.h"

namespace godot {

class PackedFloat32ArrayVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("PackedFloat32Array", Variant::PACKED_FLOAT32_ARRAY)

  PackedFloat32ArrayVariantConverter(YAML* yaml);

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;
  bool set_format(const String& format) override;

  private:
  void emit_as_sequence(ryml::NodeRef& node, const PackedFloat32Array& array) const;
};

} // namespace godot

#endif // PACKED_FLOAT32_ARRAY_YAML_H

#ifndef PACKED_INT32_ARRAY_YAML_H
#define PACKED_INT32_ARRAY_YAML_H

#include "../variant_converter.h"

namespace godot {

class PackedInt32ArrayVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("PackedInt32Array", Variant::PACKED_INT32_ARRAY)

  PackedInt32ArrayVariantConverter(YAML* yaml);

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;
  bool set_format(const String& format) override;

  private:
  void emit_as_sequence(ryml::NodeRef& node, const PackedInt32Array& array) const;
};

} // namespace godot

#endif // PACKED_INT32_ARRAY_YAML_H

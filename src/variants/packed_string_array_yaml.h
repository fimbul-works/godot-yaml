#ifndef PACKED_STRING_ARRAY_YAML_H
#define PACKED_STRING_ARRAY_YAML_H

#include "../variant_converter.h"

namespace godot {

class PackedStringArrayVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("PackedStringArray", Variant::PACKED_STRING_ARRAY)

  PackedStringArrayVariantConverter(YAML* yaml);

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;
  bool set_format(const String& format) override;

  private:
  void emit_as_sequence(ryml::NodeRef& node, const PackedStringArray& array) const;
};

} // namespace godot

#endif // PACKED_STRING_ARRAY_YAML_H

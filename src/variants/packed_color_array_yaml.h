#ifndef PACKED_COLOR_ARRAY_YAML_H
#define PACKED_COLOR_ARRAY_YAML_H

#include "../variant_converter.h"
#include "color_yaml.h"

namespace godot {

class PackedColorArrayVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("PackedColorArray", Variant::PACKED_COLOR_ARRAY)

  PackedColorArrayVariantConverter(YAML* yaml);
  ~PackedColorArrayVariantConverter();

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;
  bool set_format(const String& format) override;

  private:
  ColorVariantConverter* color_encoder;
};

} // namespace godot

#endif // PACKED_COLOR_ARRAY_YAML_H

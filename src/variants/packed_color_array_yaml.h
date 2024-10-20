#ifndef PACKED_COLOR_ARRAY_YAML_H
#define PACKED_COLOR_ARRAY_YAML_H

#include "color_yaml.h"
#include "yaml_encoder.h"

namespace godot {

class PackedColorArrayYAMLEncoder : public YAMLEncoder {
  public:
  DEFINE_YAML_TAG("PackedColorArray", Variant::PACKED_COLOR_ARRAY)

  PackedColorArrayYAMLEncoder(YAML* yaml);
  ~PackedColorArrayYAMLEncoder();

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  bool set_format(const String& format_str) override;

  ColorYAMLEncoder* color_encoder;
};

} // namespace godot

#endif // PACKED_COLOR_ARRAY_YAML_H

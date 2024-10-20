#ifndef PACKED_COLOR_ARRAY_YAML_H
#define PACKED_COLOR_ARRAY_YAML_H

#include "color_yaml.h"
#include "yaml_encoder.h"

namespace godot {

class PackedColorArrayYAMLEncoder : public IYAMLEncoder {
  public:
  const char* get_tag() const override;

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  bool set_format(const String& format_str) override;

  ColorYAMLEncoder color_encoder;
};

} // namespace godot

#endif // PACKED_COLOR_ARRAY_YAML_H

#ifndef PACKED_STRING_ARRAY_YAML_H
#define PACKED_STRING_ARRAY_YAML_H

#include "yaml_encoder.h"

namespace godot {

class PackedStringArrayYAMLEncoder : public YAMLEncoder {
  public:
  DEFINE_YAML_TAG("PackedStringArray", Variant::PACKED_STRING_ARRAY)

  PackedStringArrayYAMLEncoder(YAML* yaml);

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  bool set_format(const String& format_str) override;
};

} // namespace godot

#endif // PACKED_STRING_ARRAY_YAML_H

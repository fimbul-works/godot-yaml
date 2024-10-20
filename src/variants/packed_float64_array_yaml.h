#ifndef PACKED_FLOAT64_ARRAY_YAML_H
#define PACKED_FLOAT64_ARRAY_YAML_H

#include "yaml_encoder.h"

namespace godot {

class PackedFloat64ArrayYAMLEncoder : public YAMLEncoder {
  public:
  DEFINE_YAML_TAG("PackedFloat64Array", Variant::PACKED_FLOAT64_ARRAY)

  PackedFloat64ArrayYAMLEncoder(YAML* yaml);

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  bool set_format(const String& format_str) override;
};

} // namespace godot

#endif // PACKED_FLOAT64_ARRAY_YAML_H

#ifndef PACKED_INT64_ARRAY_YAML_H
#define PACKED_INT64_ARRAY_YAML_H

#include "yaml_encoder.h"

namespace godot {

class PackedInt64ArrayYAMLEncoder : public YAMLEncoder {
  public:
  DEFINE_YAML_TAG("PackedInt64Array", Variant::PACKED_INT64_ARRAY)

  PackedInt64ArrayYAMLEncoder(YAML* yaml);

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  bool set_format(const String& format_str) override;
};

} // namespace godot

#endif // PACKED_INT64_ARRAY_YAML_H

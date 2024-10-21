#ifndef STRING_NAME_YAML_H
#define STRING_NAME_YAML_H

#include "yaml_encoder.h"

namespace godot {

class StringNameYAMLEncoder : public YAMLEncoder {
  public:
  DEFINE_YAML_TAG("StringName", Variant::STRING_NAME)

  StringNameYAMLEncoder(YAML* yaml);

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  bool set_format(const String& format_str) override;
};

} // namespace godot

#endif // STRING_NAME_YAML_H

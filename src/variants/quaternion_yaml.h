#ifndef QUATERNION_YAML_H
#define QUATERNION_YAML_H

#include "yaml_encoder.h"

namespace godot {

class QuaternionYAMLEncoder : public YAMLEncoder {
  enum class Format {
    FLOW_MAP,
    SEQUENCE
  };

  public:
  DEFINE_YAML_TAG("Quaternion")

  QuaternionYAMLEncoder(YAML* yaml);

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  bool set_format(const String& format_str) override;

  private:
  Format format = Format::FLOW_MAP;

  void emit_as_flow(ryml::NodeRef& node, const Quaternion& vec) const;
  void emit_as_sequence(ryml::NodeRef& node, const Quaternion& vec) const;
};

} // namespace godot

#endif // QUATERNION_YAML_H

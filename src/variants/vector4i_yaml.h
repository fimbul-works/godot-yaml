#ifndef VECTOR4I_YAML_H
#define VECTOR4I_YAML_H

#include "yaml_encoder.h"

namespace godot {

class Vector4iYAMLEncoder : public IYAMLEncoder {
  enum class Format {
    FLOW_MAP,
    SEQUENCE
  };

  public:
  const char* get_tag() const override;

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  bool set_format(const String& format_str) override;

  private:
  Format format = Format::FLOW_MAP;

  void emit_as_flow(ryml::NodeRef& node, const Vector4i& vec) const;
  void emit_as_sequence(ryml::NodeRef& node, const Vector4i& vec) const;
};

} // namespace godot

#endif // VECTOR4I_YAML_H

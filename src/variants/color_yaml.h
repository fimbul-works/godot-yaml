#ifndef COLOR_YAML_H
#define COLOR_YAML_H

#include "yaml_encoder.h"

namespace godot {

class ColorYAMLEncoder : public IYAMLEncoder {
  enum class Format {
    HEX,
    HEX_STRING,
    FLOW_MAP,
    SEQUENCE
  };

  public:
  const char* get_tag() const override;

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  bool set_format(const String& format_str) override;

  private:
  Color hex_to_color(const std::string& hex) const;
  std::string ColorYAMLEncoder::color_to_hex(const Color& color, const char* prefix) const;
  void emit_as_flow(ryml::NodeRef& node, const Color& color) const;
  void emit_as_sequence(ryml::NodeRef& node, const Color& color) const;

  Format format = Format::HEX;
};

} // namespace godot

#endif // COLOR_YAML_H

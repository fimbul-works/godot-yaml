#ifndef COLOR_YAML_H
#define COLOR_YAML_H

#include "../variant_converter.h"

namespace godot {

class ColorVariantConverter : public VariantConverter {
  enum class Format {
    HEX, // 0xRRGGBB[AA]
    HEX_STRING, // #RRGGBB[AA]
    FLOW_MAP, // {r: 0, g: 0, b: 0, a: 1}
    SEQUENCE // [0, 0, 0, 1]
  };

  public:
  DEFINE_YAML_TAG("Color", Variant::COLOR)

  ColorVariantConverter(YAML* yaml);

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;
  bool set_format(const String& format) override;

  private:
  Format format = Format::HEX;

  Color hex_to_color(const std::string& hex) const;
  std::string color_to_hex(const Color& color, const char* prefix) const;
  void emit_as_flow(ryml::NodeRef& node, const Color& color) const;
  void emit_as_sequence(ryml::NodeRef& node, const Color& color) const;
};

} // namespace godot

#endif // COLOR_YAML_H

#ifndef COLOR_YAML_H
#define COLOR_YAML_H

#include "../variant_converter.h"

namespace godot {

/**
 * YAML converter for Color type. Supports the following formats:
 * - Hex number: 0xRRGGBB[AA]
 * - Hex string: #RRGGBB[AA]
 * - Flow map: {r: 0.0, g: 0.0, b: 0.0, [a: 1.0]}
 * - Sequence: [r, g, b, [a]]
 *
 * Alpha component is optional in all formats and defaults to 1.0 (255 in hex).
 */
class ColorVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("Color", Variant::COLOR)

  void encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  private:
  // Helper methods for different formats
  void emit_as_hex(ryml::NodeRef& node, const Color& color, bool with_alpha, const char* prefix) const;
  void emit_as_map(ryml::NodeRef& node, const Color& color, const YAMLFormat::View& format) const;
  void emit_as_sequence(ryml::NodeRef& node, const Color& color, const YAMLFormat::View& format) const;

  // Decoding helpers
  Variant decode_hex(const ryml::csubstr& val) const;
  Variant decode_map(const ryml::ConstNodeRef& node) const;
  Variant decode_sequence(const ryml::ConstNodeRef& node) const;
  Color parse_hex_components(const std::string& hex_str, int offset, size_t expected_length) const;

  // Utility functions
  std::string color_to_hex(const Color& color, bool with_alpha, const char* prefix) const;

  // Constants
  static constexpr int HEX_STRING_LENGTH = 7; // #RRGGBB
  static constexpr int HEX_NUMBER_LENGTH = 8; // 0xRRGGBB
  static constexpr int HEX_ALPHA_EXTRA = 2; // Additional AA
  static constexpr float COLOR_COMPONENT_MAX = 255.0f;
};

} // namespace godot

#endif // COLOR_YAML_H

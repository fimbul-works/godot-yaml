#ifndef PACKED_BYTE_ARRAY_YAML_H
#define PACKED_BYTE_ARRAY_H

#include "../variant_converter.h"
#include <string>

namespace godot {

/**
 * YAML converter for PackedByteArray type.
 * Supports two encoding formats:
 * - Hexadecimal string (auto-detected if string contains only 0-9 and A-F)
 * - Base64 string (used for all other valid encodings)
 *
 * Both formats support whitespace and line breaks for readability,
 * which are automatically stripped during decoding.
 */
class PackedByteArrayVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("PackedByteArray", Variant::PACKED_BYTE_ARRAY)

  void encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  private:
  static constexpr size_t HEX_LINE_LENGTH = 32; // Characters per line for hex format
  static constexpr size_t BASE64_LINE_LENGTH = 76; // Standard base64 line length

  // Helper struct for string cleanup and format detection
  struct CleanupResult {
    std::string cleaned;
    bool is_hex;
    size_t original_length;
  };

  // Encoding helpers
  void emit_as_hex(ryml::NodeRef& node, const PackedByteArray& array) const;
  void emit_as_base64(ryml::NodeRef& node, const PackedByteArray& array) const;

  // String processing helpers
  CleanupResult cleanup_and_detect(const ryml::csubstr& input) const;
  ryml::csubstr format_output(ryml::csubstr str, size_t line_length) const;
  PackedByteArray hex_to_bytes(const std::string& hex) const;

  // Character validation
  static bool is_hex_char(char c);
  static bool is_whitespace(char c);
};

} // namespace godot

#endif // PACKED_BYTE_ARRAY_YAML_H

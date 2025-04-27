#ifndef PACKED_BYTE_ARRAY_YAML_H
#define PACKED_BYTE_ARRAY_H

#include "../variant_converter.h"
#include <string>

namespace godot {

class PackedByteArrayVariantConverter : public VariantConverter {
public:
	DEFINE_YAML_TAG("PackedByteArray", Variant::PACKED_BYTE_ARRAY)

	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;
	Variant decode(const ryml::ConstNodeRef &node, ParserContext *context) const override;

private:
	static constexpr size_t HEX_LINE_LENGTH = 32; // Characters per line for hex format
	static constexpr size_t BASE64_LINE_LENGTH = 76; // Standard base64 line length

	// Helper struct for string cleanup and format detection
	struct CleanupResult {
		String cleaned;
		bool is_hex;
		size_t original_length;
	};

	// Encoding helpers
	void emit_as_hex(ryml::NodeRef &node, const PackedByteArray &array, const YAMLStyle::View &style) const;
	void emit_as_base64(ryml::NodeRef &node, const PackedByteArray &array, const YAMLStyle::View &style) const;

	// String processing helpers
	CleanupResult cleanup_and_detect(const ryml::csubstr &input, const ryml::ConstNodeRef &node, ParserContext *context) const;
	ryml::csubstr format_output(const String &str, size_t line_length) const;
	PackedByteArray hex_to_bytes(const String &hex) const;

	inline const bool is_base64_char(char c) const {
		return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c == '+') || (c == '/') || (c == '=');
	}
};

} // namespace godot

#endif // PACKED_BYTE_ARRAY_YAML_H

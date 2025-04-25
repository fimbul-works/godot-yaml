#include "packed_byte_array_yaml.h"
#include "../exception.h"
#include "../util_string.h"

#include <godot_cpp/classes/marshalls.hpp>

using namespace godot;

void PackedByteArrayVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const PackedByteArray array = v.operator PackedByteArray();

	if (array.size() == 0) {
		ryml::csubstr null = {};
		node << null;
		return;
	}

	if (style.get_binary_encoding() == YAMLStyle::BIN_HEX) {
		emit_as_hex(node, array, style);
	} else {
		emit_as_base64(node, array, style);
	}
}

void PackedByteArrayVariantConverter::emit_as_hex(ryml::NodeRef &node, const PackedByteArray &array, const YAMLStyle::View &style) const {
	String hex_str;

	static const char hex_chars[] = "0123456789ABCDEF";
	for (int i = 0; i < array.size(); ++i) {
		uint8_t byte = array[i];
		hex_str += hex_chars[byte >> 4];
		hex_str += hex_chars[byte & 0xF];
	}

	if (hex_str.length() > HEX_LINE_LENGTH) {
		node |= ryml::VAL_LITERAL | ryml::BLOCK;
	}

	node << format_output(hex_str, HEX_LINE_LENGTH);
}

void PackedByteArrayVariantConverter::emit_as_base64(ryml::NodeRef &node, const PackedByteArray &array, const YAMLStyle::View &style) const {
	String base64 = Marshalls::get_singleton()->raw_to_base64(array);

	if (base64.length() > BASE64_LINE_LENGTH) {
		node |= ryml::VAL_LITERAL | ryml::BLOCK;
	}

	node << format_output(base64, BASE64_LINE_LENGTH);
}

Variant PackedByteArrayVariantConverter::decode(const ryml::ConstNodeRef &node) const {
	if (node.val_is_null()) {
		return PackedByteArray();
	}

	if (!node.has_val()) {
		throw create_invalid_format_exception("PackedByteArray", node);
	}

	try {
		CleanupResult result = cleanup_and_detect(node.val(), node);

		if (result.is_hex) {
			return hex_to_bytes(result.cleaned);
		} else {
			return Marshalls::get_singleton()->base64_to_raw(result.cleaned);
		}
	} catch (const std::exception &e) {
		throw create_decode_error_exception("PackedByteArray", e.what(), node);
	}
}

PackedByteArrayVariantConverter::CleanupResult
PackedByteArrayVariantConverter::cleanup_and_detect(const ryml::csubstr &input, const ryml::ConstNodeRef &node) const {
	String cleaned;
	bool is_hex = true;

	for (size_t i = 0; i < input.len; i++) {
		char c = input.str[i];
		if (!is_whitespace(c)) {
			if (is_hex && !is_hex_char(c)) {
				is_hex = false;
			}
			cleaned += c;
		}
	}

	if (is_hex && cleaned.length() % 2 != 0) {
		throw create_exception(vformat("Invalid PackedByteArray hex string length (%d) - must be even", cleaned.length()), node);
	}

	return { std::move(cleaned), is_hex, input.len };
}

ryml::csubstr PackedByteArrayVariantConverter::format_output(const String &str, size_t line_length) const {
	if (str.length() <= line_length) {
		return store_string(str);
	}

	String formatted;

	size_t len = str.length();
	size_t pos = 0;
	while (pos < len) {
		if (pos > 0) {
			formatted += '\n';
		}
		size_t chunk_size = std::min(line_length, len - pos);
		formatted += str.substr(pos, chunk_size);
		pos += chunk_size;
	}

	// Add trailing newline to turn "|-" into just "|"
	formatted += '\n';

	return store_string(formatted);
}

PackedByteArray PackedByteArrayVariantConverter::hex_to_bytes(const String &hex) const {
	PackedByteArray array;
	array.resize(hex.length() / 2);
	const char *hex_str = hex.utf8().get_data();

	for (size_t i = 0; i < array.size(); ++i) {
		unsigned int byte;
		std::sscanf(hex_str + i * 2, "%2x", &byte);
		array.set(i, static_cast<uint8_t>(byte));
	}

	return array;
}

bool PackedByteArrayVariantConverter::is_hex_char(char c) {
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

bool PackedByteArrayVariantConverter::is_whitespace(char c) {
	return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

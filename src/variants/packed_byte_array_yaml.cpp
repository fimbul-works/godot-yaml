#include "packed_byte_array_yaml.h"
#include "../exception.h"
#include "../util_string.h"

#include <godot_cpp/classes/marshalls.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void PackedByteArrayVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const PackedByteArray array = v.operator PackedByteArray();

	if (array.size() == 0) {
		ryml::csubstr null = {};
		node << null;
		return;
	}

	// Always use literal block
	node |= ryml::VAL_LITERAL | ryml::BLOCK;

	if (style.get_binary_encoding() == YAMLStyle::BIN_HEX) {
		emit_as_hex(node, array, style);
	} else {
		emit_as_base64(node, array, style);
	}
}

void PackedByteArrayVariantConverter::emit_as_hex(ryml::NodeRef &node, const PackedByteArray &array, const YAMLStyle::View &style) const {
	String hex_str = HEX_PREFIX; // Add the hex prefix

	static const char hex_chars[] = "0123456789ABCDEF";
	for (int i = 0; i < array.size(); ++i) {
		uint8_t byte = array[i];
		hex_str += hex_chars[byte >> 4];
		hex_str += hex_chars[byte & 0xF];
	}

	node << format_output(hex_str, HEX_LINE_LENGTH);
}

void PackedByteArrayVariantConverter::emit_as_base64(ryml::NodeRef &node, const PackedByteArray &array, const YAMLStyle::View &style) const {
	String base64 = BASE64_PREFIX + Marshalls::get_singleton()->raw_to_base64(array); // Add the base64 prefix
	node << format_output(base64, BASE64_LINE_LENGTH);
}

Variant PackedByteArrayVariantConverter::decode(const ryml::ConstNodeRef &node, ParserContext *context) const {
	if (node.val_is_null()) {
		return PackedByteArray();
	}

	if (!node.has_val()) {
		throw create_invalid_format_exception(node);
	}

	try {
		CleanupResult result = cleanup_and_detect(node.val(), node, context);

		if (result.is_hex) {
			return hex_to_bytes(result.cleaned);
		} else {
			return Marshalls::get_singleton()->base64_to_raw(result.cleaned);
		}
	} catch (const std::exception &e) {
		throw create_decode_error_exception(e.what(), node);
	}
}

PackedByteArrayVariantConverter::CleanupResult
PackedByteArrayVariantConverter::cleanup_and_detect(const ryml::csubstr &input, const ryml::ConstNodeRef &node, ParserContext *context) const {
	String raw = from_ryml_str(input);
	String cleaned;
	bool is_hex = false;
	bool has_prefix = false;

	// Check if it starts with a recognized prefix
	if (raw.begins_with(HEX_PREFIX)) {
		is_hex = true;
		has_prefix = true;
		cleaned = raw.substr(String(HEX_PREFIX).length()); // Remove "hex:" prefix
	} else if (raw.begins_with(BASE64_PREFIX)) {
		is_hex = false;
		has_prefix = true;
		cleaned = raw.substr(String(BASE64_PREFIX).length()); // Remove "base64:" prefix
	} else {
		// Legacy format without prefix - try to auto-detect
		is_hex = true;
		for (int i = 0; i < raw.length(); i++) {
			char c = raw[i];
			if (!is_whitespace_char(c)) {
				if (is_hex && !is_hex_char(c)) {
					is_hex = false;
					if (!is_base64_char(c)) {
						throw create_exception(vformat("Invalid character '%c' in PackedByteArray - format is ambiguous, use '%s' or '%s' prefix", c, HEX_PREFIX, BASE64_PREFIX), node);
					}
				}
				cleaned += c;
			}
		}
	}

	// Remove whitespace if we have a prefix
	if (has_prefix) {
		String temp = cleaned;
		cleaned = "";
		for (int i = 0; i < temp.length(); i++) {
			if (!is_whitespace_char(temp[i])) {
				cleaned += temp[i];
			}
		}
	}

	// Validate hex format
	if (is_hex && cleaned.length() % 2 != 0) {
		throw create_exception(vformat("Invalid PackedByteArray hex string length (%d) - must be even", cleaned.length()), node);
	}

	// Set style if style detection is enabled
	if (context->detect_style) {
		context->current_style()->set_binary_encoding(is_hex ? YAMLStyle::BIN_HEX : YAMLStyle::BIN_BASE64);
	}

	return { std::move(cleaned), is_hex, input.len, has_prefix };
}

ryml::csubstr PackedByteArrayVariantConverter::format_output(const String &str, size_t line_length) const {
	String formatted;
	String prefix = "";
	String content = str;

	// Extract prefix if present
	if (str.begins_with(HEX_PREFIX)) {
		prefix = HEX_PREFIX;
		content = str.substr(String(HEX_PREFIX).length());
	} else if (str.begins_with(BASE64_PREFIX)) {
		prefix = BASE64_PREFIX;
		content = str.substr(String(BASE64_PREFIX).length());
	}

	// Add the prefix once at the beginning if we want to preserve it
	if (!prefix.is_empty()) {
		formatted += prefix;
	}

	// Break the content into chunks without adding prefix to continuation lines
	size_t pos = 0;
	size_t first_line_max;

	// Calculate how much content can fit on the first line
	if (!prefix.is_empty()) {
		// First line has less space due to prefix
		first_line_max = (prefix.length() < line_length) ? line_length - prefix.length() : 0;
	} else {
		// No prefix or not preserving it, so full line length is available
		first_line_max = line_length;
	}

	// Add first chunk
	size_t chunk_size = std::min(first_line_max, (size_t)content.length());
	formatted += content.substr(0, chunk_size);
	pos += chunk_size;

	// Add remaining chunks
	while (pos < content.length()) {
		formatted += '\n';
		chunk_size = std::min(line_length, (size_t)content.length() - pos);
		formatted += content.substr(pos, chunk_size);
		pos += chunk_size;
	}

	// For literal block style, ensure it ends with a newline
	formatted += '\n';

	return store_string(formatted);
}

PackedByteArray PackedByteArrayVariantConverter::hex_to_bytes(const String &hex) const {
	PackedByteArray array;
	array.resize(hex.length() / 2);
	const char *hex_str = hex.utf8().get_data();

	for (size_t i = 0; i < array.size(); ++i) {
		unsigned int byte;
		sscanf(hex_str + i * 2, "%2x", &byte);
		array.set(i, static_cast<uint8_t>(byte));
	}

	return array;
}

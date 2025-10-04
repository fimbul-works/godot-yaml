#include "packed_byte_array_variant_converter.hpp"
#include "../exception.hpp"
#include "../util/util_string.hpp"

#include <godot_cpp/classes/marshalls.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void YAMLPackedByteArrayVariantConverter::encode(
		ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const PackedByteArray array = v.operator PackedByteArray();

	if (array.size() == 0) {
		ryml::csubstr null = {};
		node << null;
		return;
	}

	// Always use literal block
	node |= ryml::VAL_LITERAL | ryml::BLOCK;

	String base64 = Marshalls::get_singleton()->raw_to_base64(array);
	node << format_output(base64, BASE64_LINE_LENGTH);
}

Variant YAMLPackedByteArrayVariantConverter::decode(const ryml::ConstNodeRef &node, YAMLParserContext *context) const {
	if (node.val_is_null()) {
		return PackedByteArray();
	}

	if (!node.has_val()) {
		throw create_invalid_format_exception(node);
	}

	try {
		return Marshalls::get_singleton()->base64_to_raw(cleanup(node.val(), node, context));
	} catch (const std::exception &e) {
		throw create_decode_error_exception(e.what(), node);
	}
}

String YAMLPackedByteArrayVariantConverter::cleanup(
		const ryml::csubstr &input, const ryml::ConstNodeRef &node, YAMLParserContext *context) const {
	String raw = from_ryml_str(input);
	String cleaned;

	for (int i = 0; i < raw.length(); i++) {
		char c = raw[i];
		if (!is_whitespace_char(c)) {
			if (!is_base64_char(c)) {
				throw create_exception(vformat("Invalid character '%c' in PackedByteArray", c), node);
			}
			cleaned += c;
		}
	}

	return cleaned;
}

ryml::csubstr YAMLPackedByteArrayVariantConverter::format_output(const String &str, size_t line_length) const {
	String formatted;
	String prefix = "";
	String content = str;

	// Break the content into chunks without adding prefix to continuation lines
	size_t pos = 0;

	// Add first chunk
	size_t chunk_size = std::min(line_length, (size_t)content.length());
	formatted += content.substr(0, chunk_size);
	pos += chunk_size;

	// Add remaining chunks
	while (pos < content.length()) {
		formatted += '\n';
		chunk_size = std::min(line_length, (size_t)content.length() - pos);
		formatted += content.substr(pos, chunk_size);
		pos += chunk_size;
	}

	// For literal block style, ensure it ends with a newline to avoid |-
	formatted += '\n';

	return store_string(formatted);
}

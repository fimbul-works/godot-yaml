#include "color_yaml.h"
#include "../exception.h"
#include "../util_numeric.h"

using namespace godot;

void ColorVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const Color color = v.operator Color();
	const bool has_alpha = color.a < 1.0f;

	if (style.get_integer_format() == YAMLStyle::INT_HEX) {
		emit_as_hex(node, color, has_alpha, "0x");
	} else if (style.get_binary_encoding() == YAMLStyle::BIN_HEX) {
		emit_as_hex(node, color, has_alpha, "#");
	} else if (style.get_container_form() == YAMLStyle::FORM_SEQ) {
		emit_as_sequence(node, color, style);
	} else {
		emit_as_map(node, color, style);
	}
}

void ColorVariantConverter::emit_as_hex(ryml::NodeRef &node, const Color &color, bool with_alpha, const char *prefix) const {
	node << color_to_hex(color, with_alpha, prefix);
}

void ColorVariantConverter::emit_as_map(ryml::NodeRef &node, const Color &color, const YAMLStyle::View &style) const {
	node |= ryml::MAP;

	style.apply_flow_style(node);

	YAMLStyle::FloatFormat float_format = style.get_float_format();
	node["r"] << float_to_string(color.r, float_format);
	node["g"] << float_to_string(color.g, float_format);
	node["b"] << float_to_string(color.b, float_format);
	if (color.a < 1.0f) {
		node["a"] << float_to_string(color.a, float_format);
	}
}

void ColorVariantConverter::emit_as_sequence(ryml::NodeRef &node, const Color &color, const YAMLStyle::View &style) const {
	node |= ryml::SEQ;

	style.apply_flow_style(node);

	YAMLStyle::FloatFormat float_format = style.is_valid() ? style.get_float_format() : YAMLStyle::FLOAT_ANY;
	node.append_child() << float_to_string(color.r, float_format);
	node.append_child() << float_to_string(color.g, float_format);
	node.append_child() << float_to_string(color.b, float_format);
	if (color.a < 1.0f) {
		node.append_child() << float_to_string(color.a, float_format);
	}
}

Variant ColorVariantConverter::decode(const ryml::ConstNodeRef &node) const {
	try {
		if (node.has_val() && !node.val_is_null()) {
			if (node.val().begins_with("0x") || node.val().begins_with("#")) {
				return decode_hex(node);
			}
		}

		if (node.is_map()) {
			return decode_map(node);
		}

		if (node.is_seq()) {
			return decode_sequence(node);
		}

		throw create_invalid_format_exception("Color", node);
	} catch (const YAMLException &) {
		throw; // Re-throw YAML exceptions
	} catch (const std::exception &e) {
		throw create_decode_error_exception("Color", e.what(), node);
	}
}

Color ColorVariantConverter::parse_hex_components(const String &hex_str, int offset, size_t expected_length, const ryml::ConstNodeRef &node) const {
	bool has_alpha = hex_str.length() == expected_length + 2;

	if (hex_str.length() < offset + 6 + (has_alpha ? 2 : 0)) {
		throw create_exception("Invalid hex color length", node);
	}

	try {
		for (int i = offset; i < offset + 6 + (has_alpha ? 2 : 0); ++i) {
			if (!is_hex_char(hex_str[i])) {
				throw create_exception(vformat("Invalid hex color: %s", hex_str), node);
			}
		}

		int r = hex_str.substr(offset, 2).hex_to_int();
		int g = hex_str.substr(offset + 2, 2).hex_to_int();
		int b = hex_str.substr(offset + 4, 2).hex_to_int();
		int a = has_alpha ? hex_str.substr(offset + 6, 2).hex_to_int() : 255;

		return Color(
				static_cast<float>(r) / COLOR_COMPONENT_MAX,
				static_cast<float>(g) / COLOR_COMPONENT_MAX,
				static_cast<float>(b) / COLOR_COMPONENT_MAX,
				static_cast<float>(a) / COLOR_COMPONENT_MAX);
	} catch (const YAMLException &) {
		throw; // Re-throw YAML exceptions
	} catch (const std::exception &e) {
		throw create_exception(vformat("Invalid hex color format: %s", e.what()), node);
	}
}

Variant ColorVariantConverter::decode_hex(const ryml::ConstNodeRef &node) const {
	String hex_str = from_ryml_str(node.val());

	try {
		if (hex_str[0] == '#') {
			return parse_hex_components(hex_str, 1, HEX_STRING_LENGTH, node);
		} else if (hex_str.begins_with("0x")) {
			return parse_hex_components(hex_str, 2, HEX_NUMBER_LENGTH, node);
		}
		throw create_exception("Invalid hex color format", node);
	} catch (const YAMLException &) {
		throw;
	} catch (const std::exception &e) {
		throw create_exception(vformat("Failed to parse hex color: %s", e.what()), node);
	}
}

Variant ColorVariantConverter::decode_map(const ryml::ConstNodeRef &node) const {
	check_required_fields(node, { "r", "g", "b" });

	real_t r = string_to_float<real_t>(node["r"].val());
	real_t g = string_to_float<real_t>(node["g"].val());
	real_t b = string_to_float<real_t>(node["b"].val());
	real_t a = node.has_child("a") ? string_to_float<real_t>(node["a"].val()) : 1.0f;

	if (r < 0.0f) {
		throw create_exception("Negative color component value (r)", node);
	}

	if (g < 0.0f) {
		throw create_exception("Negative color component value (g)", node);
	}

	if (b < 0.0f) {
		throw create_exception("Negative color component value (b)", node);
	}

	if (a < 0.0f) {
		throw create_exception("Negative color component value (a)", node);
	}

	return Color(r, g, b, a);
}

Variant ColorVariantConverter::decode_sequence(const ryml::ConstNodeRef &node) const {
	const size_t size = node.num_children();
	if (size != 3 && size != 4) {
		throw create_exception("Color sequence must have 3 or 4 elements (RGB[A])", node);
	}

	real_t r = string_to_float<real_t>(node[0].val());
	real_t g = string_to_float<real_t>(node[1].val());
	real_t b = string_to_float<real_t>(node[2].val());
	real_t a = size == 4 ? string_to_float<real_t>(node[3].val()) : 1.0f;

	return Color(r, g, b, a);
}

ryml::csubstr ColorVariantConverter::color_to_hex(const Color &color, bool with_alpha, const char *prefix) const {
	uint8_t r = static_cast<uint8_t>(CLAMP(color.r * COLOR_COMPONENT_MAX, 0.0f, COLOR_COMPONENT_MAX));
	uint8_t g = static_cast<uint8_t>(CLAMP(color.g * COLOR_COMPONENT_MAX, 0.0f, COLOR_COMPONENT_MAX));
	uint8_t b = static_cast<uint8_t>(CLAMP(color.b * COLOR_COMPONENT_MAX, 0.0f, COLOR_COMPONENT_MAX));

	if (with_alpha) {
		uint8_t a = static_cast<uint8_t>(CLAMP(color.a * COLOR_COMPONENT_MAX, 0.0f, COLOR_COMPONENT_MAX));
		return string_pool.store(vformat("%s%02X%02X%02X%02X", prefix, r, g, b, a));
	}

	return string_pool.store(vformat("%s%02X%02X%02X", prefix, r, g, b));
}

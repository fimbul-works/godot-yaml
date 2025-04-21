#include "color_yaml.h"
#include "../exception.h"
#include "../util_numeric.h"

using namespace godot;

void ColorVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const Color color = v.operator Color();
	const bool has_alpha = color.a < 1.0f;

	if (style.get_number_format() == YAMLStyle::NUM_HEX) {
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
	// Map styles
	node |= ryml::MAP;

	// Flow style
	if (!style.is_valid()) {
		node |= ryml::FLOW_SL;
	} else {
		style.apply_flow_style(node);
	}

	node["r"] << float_to_string(color.r);
	node["g"] << float_to_string(color.g);
	node["b"] << float_to_string(color.b);
	if (color.a < 1.0f) {
		node["a"] << float_to_string(color.a);
	}
}

void ColorVariantConverter::emit_as_sequence(ryml::NodeRef &node, const Color &color, const YAMLStyle::View &style) const {
	node |= ryml::SEQ;

	// Flow style
	if (!style.is_valid()) {
		node |= ryml::FLOW_SL;
	} else {
		style.apply_flow_style(node);
	}

	node.append_child() << float_to_string(color.r);
	node.append_child() << float_to_string(color.g);
	node.append_child() << float_to_string(color.b);
	if (color.a < 1.0f) {
		node.append_child() << float_to_string(color.a);
	}
}

Variant ColorVariantConverter::decode(const ryml::ConstNodeRef &node) const {
	try {
		if (node.has_val() && !node.val_is_null()) {
			if (node.val().begins_with("0x") || node.val().begins_with("#")) {
				return decode_hex(node.val());
			}
		}

		if (node.is_map()) {
			return decode_map(node);
		}

		if (node.is_seq()) {
			return decode_sequence(node);
		}

		throw YAMLException::create_invalid_format("Color");
	} catch (const YAMLException &) {
		throw;
	} catch (const std::exception &e) {
		throw YAMLException::create_decode_error("Color", e.what());
	}
}

Color ColorVariantConverter::parse_hex_components(const String &hex_str, int offset, size_t expected_length) const {
	if (hex_str.length() != expected_length && hex_str.length() != expected_length + HEX_ALPHA_EXTRA) {
		throw YAMLException("Invalid hex color length");
	}

	try {
		int r = hex_str.substr(offset, 2).hex_to_int();
		int g = hex_str.substr(offset + 2, 2).hex_to_int();
		int b = hex_str.substr(offset + 4, 2).hex_to_int();
		int a = hex_str.length() == expected_length + HEX_ALPHA_EXTRA ? hex_str.substr(offset + 6, 2).hex_to_int() : 255;

		return Color(r / COLOR_COMPONENT_MAX,
				g / COLOR_COMPONENT_MAX,
				b / COLOR_COMPONENT_MAX,
				a / COLOR_COMPONENT_MAX);
	} catch (...) {
		throw YAMLException("Invalid hex color component");
	}
}

Variant ColorVariantConverter::decode_hex(const ryml::csubstr &val) const {
	String hex_str = from_ryml_str(val);

	try {
		if (hex_str[0] == '#') {
			return parse_hex_components(hex_str, 1, HEX_STRING_LENGTH);
		} else if (hex_str.begins_with("0x")) {
			return parse_hex_components(hex_str, 2, HEX_NUMBER_LENGTH);
		}
		throw YAMLException("Invalid hex color format");
	} catch (const YAMLException &) {
		throw;
	} catch (const std::exception &e) {
		throw YAMLException(String("Failed to parse hex color: ") + e.what());
	}
}

Variant ColorVariantConverter::decode_map(const ryml::ConstNodeRef &node) const {
	if (!node.has_child("r") || !node.has_child("g") || !node.has_child("b")) {
		throw YAMLException::create_missing_field("Color", "r, g, b");
	}

	real_t r = string_to_float<real_t>(node["r"].val());
	real_t g = string_to_float<real_t>(node["g"].val());
	real_t b = string_to_float<real_t>(node["b"].val());
	real_t a = node.has_child("a") ? string_to_float<real_t>(node["a"].val()) : 1.0f;

	return Color(r, g, b, a);
}

Variant ColorVariantConverter::decode_sequence(const ryml::ConstNodeRef &node) const {
	const size_t size = node.num_children();
	if (size != 3 && size != 4) {
		throw YAMLException("Color sequence must have 3 or 4 elements (RGB[A])");
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

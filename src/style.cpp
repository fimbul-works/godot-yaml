#include "style.h"
#include "util_string.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <ryml.hpp>

using namespace godot;

YAMLStyle::YAMLStyle() :
		scalar_style(SCALAR_ANY),
		quote_style(QUOTE_ANY),
		container_form(FORM_ANY),
		flow_style(FLOW_ANY),
		number_format(NUM_ANY),
		binary_encoding(BIN_ANY) {
}

bool YAMLStyle::is_block_style() const {
	return scalar_style == SCALAR_BLOCK || scalar_style == SCALAR_LITERAL || scalar_style == SCALAR_FOLDED;
}

bool YAMLStyle::uses_quotes() const {
	return quote_style == QUOTE_SINGLE || quote_style == QUOTE_DOUBLE;
}

bool YAMLStyle::uses_flow() const {
	return flow_style == FLOW_SINGLE;
}

void YAMLStyle::_bind_methods() {
	// Bind enums
	BIND_ENUM_CONSTANT(SCALAR_ANY);
	BIND_ENUM_CONSTANT(SCALAR_PLAIN);
	BIND_ENUM_CONSTANT(SCALAR_BLOCK);
	BIND_ENUM_CONSTANT(SCALAR_LITERAL);
	BIND_ENUM_CONSTANT(SCALAR_FOLDED);

	BIND_ENUM_CONSTANT(QUOTE_ANY);
	BIND_ENUM_CONSTANT(QUOTE_NONE);
	BIND_ENUM_CONSTANT(QUOTE_SINGLE);
	BIND_ENUM_CONSTANT(QUOTE_DOUBLE);

	BIND_ENUM_CONSTANT(FORM_ANY);
	BIND_ENUM_CONSTANT(FORM_SEQ);
	BIND_ENUM_CONSTANT(FORM_MAP);

	BIND_ENUM_CONSTANT(FLOW_ANY);
	BIND_ENUM_CONSTANT(FLOW_NONE);
	BIND_ENUM_CONSTANT(FLOW_SINGLE);

	BIND_ENUM_CONSTANT(NUM_ANY);
	BIND_ENUM_CONSTANT(NUM_DECIMAL);
	BIND_ENUM_CONSTANT(NUM_HEX);
	BIND_ENUM_CONSTANT(NUM_OCTAL);
	BIND_ENUM_CONSTANT(NUM_BINARY);
	BIND_ENUM_CONSTANT(NUM_SCIENTIFIC);

	BIND_ENUM_CONSTANT(BIN_ANY);
	BIND_ENUM_CONSTANT(BIN_STRING);
	BIND_ENUM_CONSTANT(BIN_BASE64);
	BIND_ENUM_CONSTANT(BIN_HEX);

	// Bind methods
	ClassDB::bind_method(D_METHOD("set_scalar_style", "style"), &YAMLStyle::set_scalar_style);
	ClassDB::bind_method(D_METHOD("get_scalar_style"), &YAMLStyle::get_scalar_style);
	ClassDB::bind_method(D_METHOD("set_quote_style", "style"), &YAMLStyle::set_quote_style);
	ClassDB::bind_method(D_METHOD("get_quote_style"), &YAMLStyle::get_quote_style);
	ClassDB::bind_method(D_METHOD("set_container_form", "style"), &YAMLStyle::set_container_form);
	ClassDB::bind_method(D_METHOD("get_container_form"), &YAMLStyle::get_container_form);
	ClassDB::bind_method(D_METHOD("set_flow_style", "style"), &YAMLStyle::set_flow_style);
	ClassDB::bind_method(D_METHOD("get_flow_style"), &YAMLStyle::get_flow_style);
	ClassDB::bind_method(D_METHOD("set_number_format", "format"), &YAMLStyle::set_number_format);
	ClassDB::bind_method(D_METHOD("get_number_format"), &YAMLStyle::get_number_format);
	ClassDB::bind_method(D_METHOD("set_binary_encoding", "encoding"), &YAMLStyle::set_binary_encoding);
	ClassDB::bind_method(D_METHOD("get_binary_encoding"), &YAMLStyle::get_binary_encoding);
	ClassDB::bind_method(D_METHOD("set_custom_settings", "style"), &YAMLStyle::set_custom_settings);
	ClassDB::bind_method(D_METHOD("get_custom_settings"), &YAMLStyle::get_custom_settings);

	ClassDB::bind_method(D_METHOD("is_block_style"), &YAMLStyle::is_block_style);
	ClassDB::bind_method(D_METHOD("uses_quotes"), &YAMLStyle::uses_quotes);
	ClassDB::bind_method(D_METHOD("uses_flow"), &YAMLStyle::uses_flow);

	ClassDB::bind_method(D_METHOD("get_child", "key"), &YAMLStyle::get_child);
	ClassDB::bind_method(D_METHOD("set_child", "key", "style"), &YAMLStyle::set_child);
	ClassDB::bind_method(D_METHOD("has_child", "key"), &YAMLStyle::has_child);
	ClassDB::bind_method(D_METHOD("clear_child", "key"), &YAMLStyle::clear_child);
	ClassDB::bind_method(D_METHOD("clear_children"), &YAMLStyle::clear_children);
	ClassDB::bind_method(D_METHOD("get_children_keys"), &YAMLStyle::get_children_keys);

	ClassDB::bind_method(D_METHOD("get_debug_string"), &YAMLStyle::get_debug_string);
	ClassDB::bind_static_method("YAMLStyle", D_METHOD("get_scalar_style_string", "p_style"), &YAMLStyle::get_scalar_style_string);
	ClassDB::bind_static_method("YAMLStyle", D_METHOD("get_quote_style_string", "p_style"), &YAMLStyle::get_quote_style_string);
	ClassDB::bind_static_method("YAMLStyle", D_METHOD("get_container_form_string", "p_style"), &YAMLStyle::get_container_form_string);
	ClassDB::bind_static_method("YAMLStyle", D_METHOD("get_flow_style_string", "p_style"), &YAMLStyle::get_flow_style_string);
	ClassDB::bind_static_method("YAMLStyle", D_METHOD("get_number_format_string", "p_style"), &YAMLStyle::get_number_format_string);
	ClassDB::bind_static_method("YAMLStyle", D_METHOD("get_binary_encoding_string", "p_style"), &YAMLStyle::get_binary_encoding_string);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "scalar_style", PROPERTY_HINT_ENUM, "Any,Plain,Block,Literal,Folded"), "set_scalar_style", "get_scalar_style");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "quote_style", PROPERTY_HINT_ENUM, "Any,None,Single,Double"), "set_quote_style", "get_quote_style");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "container_form", PROPERTY_HINT_ENUM, "Any,Sequence,Mapping"), "set_container_form", "get_container_form");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "flow_style", PROPERTY_HINT_ENUM, "Any,None,Single"), "set_flow_style", "get_flow_style");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "number_format", PROPERTY_HINT_ENUM, "Any,Decimal,Hex,Octal,Binary,Scientific"), "set_number_format", "get_number_format");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "binary_encoding", PROPERTY_HINT_ENUM, "Any,String,Base64,Hex"), "set_binary_encoding", "get_binary_encoding");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "custom_settings"), "set_custom_settings", "get_custom_settings");
}

// Child style management implementations
Ref<YAMLStyle> YAMLStyle::get_child(const String &key) const {
	auto it = child_styles.find(key);
	return it != child_styles.end() ? it->second : Ref<YAMLStyle>();
}

void YAMLStyle::set_child(const String &key, const Ref<YAMLStyle> &style) {
	if (style.is_valid()) {
		child_styles[key] = style;
	} else {
		child_styles.erase(key);
	}
}

bool YAMLStyle::has_child(const String &key) const {
	return child_styles.find(key) != child_styles.end();
}

void YAMLStyle::clear_child(const String &key) {
	child_styles.erase(key);
}

void YAMLStyle::clear_children() {
	child_styles.clear();
}

Array YAMLStyle::get_children_keys() const {
	Array keys;
	for (const auto &pair : child_styles) {
		keys.push_back(pair.first);
	}
	return keys;
}

// Debug string helpers
String YAMLStyle::get_scalar_style_string(ScalarStyle p_style) {
	switch (p_style) {
		case SCALAR_ANY:
			return "Any";
		case SCALAR_PLAIN:
			return "Plain";
		case SCALAR_BLOCK:
			return "Block";
		case SCALAR_LITERAL:
			return "Literal";
		case SCALAR_FOLDED:
			return "Folded";
		default:
			return "Unknown";
	}
}

String YAMLStyle::get_quote_style_string(QuoteStyle p_style) {
	switch (p_style) {
		case QUOTE_ANY:
			return "Any";
		case QUOTE_NONE:
			return "None";
		case QUOTE_SINGLE:
			return "Single";
		case QUOTE_DOUBLE:
			return "Double";
		default:
			return "Unknown";
	}
}

String YAMLStyle::get_container_form_string(ContainerForm p_style) {
	switch (p_style) {
		case FORM_ANY:
			return "Any";
		case FORM_SEQ:
			return "Sequence";
		case FORM_MAP:
			return "Mapping";
		default:
			return "Unknown";
	}
}

String YAMLStyle::get_flow_style_string(FlowStyle p_style) {
	switch (p_style) {
		case FLOW_ANY:
			return "Any";
		case FLOW_NONE:
			return "None";
		case FLOW_SINGLE:
			return "Single";
		default:
			return "Unknown";
	}
}

String YAMLStyle::get_number_format_string(NumberFormat p_format) {
	switch (p_format) {
		case NUM_ANY:
			return "Any";
		case NUM_DECIMAL:
			return "Decimal";
		case NUM_HEX:
			return "Hex";
		case NUM_OCTAL:
			return "Octal";
		case NUM_BINARY:
			return "Binary";
		case NUM_SCIENTIFIC:
			return "Scientific";
		default:
			return "Unknown";
	}
}

String YAMLStyle::get_binary_encoding_string(BinaryEncoding p_encoding) {
	switch (p_encoding) {
		case BIN_ANY:
			return "Any";
		case BIN_STRING:
			return "String";
		case BIN_BASE64:
			return "Base64";
		case BIN_HEX:
			return "Hex";
		default:
			return "Unknown";
	}
}

String YAMLStyle::get_debug_string() const {
	String debug;
	debug += "YAML Style Configuration:\n";
	debug += "-----------------------\n";
	debug += vformat("Scalar Style:     %s (%s)\n", get_scalar_style_string(scalar_style), has_scalar_style ? "Explicit" : "Inherited");
	debug += vformat("Quote Style:      %s (%s)\n", get_quote_style_string(quote_style), has_quote_style ? "Explicit" : "Inherited");
	debug += vformat("Collection Style: %s (%s)\n", get_container_form_string(container_form), has_container_form ? "Explicit" : "Inherited");
	debug += vformat("Flow Style:       %s (%s)\n", get_flow_style_string(flow_style), has_flow_style ? "Explicit" : "Inherited");
	debug += vformat("Number Format:    %s (%s)\n", get_number_format_string(number_format), has_number_format ? "Explicit" : "Inherited");
	debug += vformat("Binary Encoding:  %s (%s)\n", get_binary_encoding_string(binary_encoding), has_binary_encoding ? "Explicit" : "Inherited");

	if (!custom_settings.is_empty()) {
		debug += "\nCustom Settings:\n";
		Array keys = custom_settings.keys();
		for (int i = 0; i < keys.size(); i++) {
			debug += vformat("  %s: %s\n", String(keys[i]), String(custom_settings[keys[i]]));
		}
	}

	if (!child_styles.empty()) {
		debug += "\nChild Styles:\n";
		for (const auto &pair : child_styles) {
			debug += vformat("  %s:\n", pair.first);
			String child_debug = pair.second->get_debug_string();
			PackedStringArray lines = child_debug.split("\n");
			for (int i = 0; i < lines.size(); i++) {
				if (!lines[i].is_empty()) {
					debug += vformat("    %s\n", lines[i]);
				}
			}
		}
	}

	return debug;
}

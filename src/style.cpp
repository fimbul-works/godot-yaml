#include "style.h"
#include "util_string.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <ryml.hpp>

using namespace godot;

YAMLStyle::YAMLStyle() :
		container_form(FORM_ANY),
		flow_style(FLOW_ANY),
		string_style(STRING_ANY),
		integer_format(INT_ANY),
		float_format(FLOAT_ANY),
		binary_encoding(BIN_ANY) {
}

bool YAMLStyle::is_block_style() const {
	return string_style == STRING_LITERAL || string_style == STRING_FOLDED;
}

bool YAMLStyle::uses_quotes() const {
	return string_style == STRING_QUOTE_SINGLE || string_style == STRING_QUOTE_DOUBLE;
}

bool YAMLStyle::uses_flow() const {
	return flow_style == FLOW_SINGLE;
}

void YAMLStyle::_bind_methods() {
	// Bind enums
	BIND_ENUM_CONSTANT(FORM_ANY);
	BIND_ENUM_CONSTANT(FORM_SEQ);
	BIND_ENUM_CONSTANT(FORM_MAP);

	BIND_ENUM_CONSTANT(FLOW_ANY);
	BIND_ENUM_CONSTANT(FLOW_NONE);
	BIND_ENUM_CONSTANT(FLOW_SINGLE);

	BIND_ENUM_CONSTANT(STRING_ANY);
	BIND_ENUM_CONSTANT(STRING_PLAIN);
	BIND_ENUM_CONSTANT(STRING_QUOTE_SINGLE);
	BIND_ENUM_CONSTANT(STRING_QUOTE_DOUBLE);
	BIND_ENUM_CONSTANT(STRING_LITERAL);
	BIND_ENUM_CONSTANT(STRING_FOLDED);

	BIND_ENUM_CONSTANT(INT_ANY);
	BIND_ENUM_CONSTANT(INT_DECIMAL);
	BIND_ENUM_CONSTANT(INT_HEX);
	BIND_ENUM_CONSTANT(INT_OCTAL);
	BIND_ENUM_CONSTANT(INT_BINARY);
	BIND_ENUM_CONSTANT(INT_SCIENTIFIC);

	BIND_ENUM_CONSTANT(FLOAT_ANY);
	BIND_ENUM_CONSTANT(FLOAT_DECIMAL);
	BIND_ENUM_CONSTANT(FLOAT_SCIENTIFIC);

	BIND_ENUM_CONSTANT(BIN_ANY);
	BIND_ENUM_CONSTANT(BIN_BASE64);
	BIND_ENUM_CONSTANT(BIN_HEX);

	// Bind methods
	ClassDB::bind_method(D_METHOD("set_container_form", "style"), &YAMLStyle::set_container_form);
	ClassDB::bind_method(D_METHOD("get_container_form"), &YAMLStyle::get_container_form);
	ClassDB::bind_method(D_METHOD("set_flow_style", "style"), &YAMLStyle::set_flow_style);
	ClassDB::bind_method(D_METHOD("get_flow_style"), &YAMLStyle::get_flow_style);
	ClassDB::bind_method(D_METHOD("set_string_style", "style"), &YAMLStyle::set_string_style);
	ClassDB::bind_method(D_METHOD("get_string_style"), &YAMLStyle::get_string_style);
	ClassDB::bind_method(D_METHOD("set_integer_format", "format"), &YAMLStyle::set_integer_format);
	ClassDB::bind_method(D_METHOD("get_integer_format"), &YAMLStyle::get_integer_format);
	ClassDB::bind_method(D_METHOD("set_float_format", "format"), &YAMLStyle::set_float_format);
	ClassDB::bind_method(D_METHOD("get_float_format"), &YAMLStyle::get_float_format);
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
	ClassDB::bind_static_method("YAMLStyle", D_METHOD("get_container_form_string", "p_style"), &YAMLStyle::get_container_form_string);
	ClassDB::bind_static_method("YAMLStyle", D_METHOD("get_flow_style_string", "p_style"), &YAMLStyle::get_flow_style_string);
	ClassDB::bind_static_method("YAMLStyle", D_METHOD("get_string_style_string", "p_style"), &YAMLStyle::get_string_style_string);
	ClassDB::bind_static_method("YAMLStyle", D_METHOD("get_integer_format_string", "p_style"), &YAMLStyle::get_integer_format_string);
	ClassDB::bind_static_method("YAMLStyle", D_METHOD("get_float_format_string", "p_style"), &YAMLStyle::get_float_format_string);
	ClassDB::bind_static_method("YAMLStyle", D_METHOD("get_binary_encoding_string", "p_style"), &YAMLStyle::get_binary_encoding_string);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "container_form", PROPERTY_HINT_ENUM, "Any,Sequence,Mapping"), "set_container_form", "get_container_form");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "flow_style", PROPERTY_HINT_ENUM, "Any,None,Single"), "set_flow_style", "get_flow_style");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "string_style", PROPERTY_HINT_ENUM, "Any,Plain,Single Quoted,Double Quoted,Literal,Folded"), "set_string_style", "get_string_style");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "integer_format", PROPERTY_HINT_ENUM, "Any,Decimal,Hex,Octal,Binary,Scientific"), "set_integer_format", "get_integer_format");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "float_format", PROPERTY_HINT_ENUM, "Any,Decimal,Scientific"), "set_float_format", "get_float_format");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "binary_encoding", PROPERTY_HINT_ENUM, "Any,Base64,Hex"), "set_binary_encoding", "get_binary_encoding");
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

// Debug string helpers
String YAMLStyle::get_string_style_string(StringStyle p_style) {
	switch (p_style) {
		case STRING_ANY:
			return "Any";
		case STRING_PLAIN:
			return "Plain";
		case STRING_QUOTE_SINGLE:
			return "Single Quoted";
		case STRING_QUOTE_DOUBLE:
			return "Double Quoted";
		case STRING_LITERAL:
			return "Literal";
		case STRING_FOLDED:
			return "Folded";
		default:
			return "Unknown";
	}
}

String YAMLStyle::get_integer_format_string(IntegerFormat p_format) {
	switch (p_format) {
		case INT_ANY:
			return "Any";
		case INT_DECIMAL:
			return "Decimal";
		case INT_HEX:
			return "Hex";
		case INT_OCTAL:
			return "Octal";
		case INT_BINARY:
			return "Binary";
		case INT_SCIENTIFIC:
			return "Scientific";
		default:
			return "Unknown";
	}
}

String YAMLStyle::get_float_format_string(FloatFormat p_format) {
	switch (p_format) {
		case FLOAT_ANY:
			return "Any";
		case FLOAT_DECIMAL:
			return "Decimal";
		case FLOAT_SCIENTIFIC:
			return "Scientific";
		default:
			return "Unknown";
	}
}

String YAMLStyle::get_binary_encoding_string(BinaryEncoding p_encoding) {
	switch (p_encoding) {
		case BIN_ANY:
			return "Any";
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
	debug += vformat("Collection Style: %s (%s)\n", get_container_form_string(container_form), has_container_form ? "Explicit" : "Inherited");
	debug += vformat("Flow Style:       %s (%s)\n", get_flow_style_string(flow_style), has_flow_style ? "Explicit" : "Inherited");
	debug += vformat("String Style:     %s (%s)\n", get_string_style_string(string_style), has_string_style ? "Explicit" : "Inherited");
	debug += vformat("Integer Format:   %s (%s)\n", get_integer_format_string(integer_format), has_integer_format ? "Explicit" : "Inherited");
	debug += vformat("Float Format:     %s (%s)\n", get_float_format_string(float_format), has_float_format ? "Explicit" : "Inherited");
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

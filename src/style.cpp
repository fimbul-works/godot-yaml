#include "style.h"
#include "util_string.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/templates/hashfuncs.hpp>
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

	BIND_ENUM_CONSTANT(FLOAT_ANY);
	BIND_ENUM_CONSTANT(FLOAT_DECIMAL);
	BIND_ENUM_CONSTANT(FLOAT_SCIENTIFIC);

	BIND_ENUM_CONSTANT(BIN_ANY);
	BIND_ENUM_CONSTANT(BIN_BASE64);
	BIND_ENUM_CONSTANT(BIN_HEX);

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
	ClassDB::bind_static_method("YAMLStyle", D_METHOD("container_form_string", "p_style"), &YAMLStyle::container_form_string);
	ClassDB::bind_static_method("YAMLStyle", D_METHOD("flow_style_string", "p_style"), &YAMLStyle::flow_style_string);
	ClassDB::bind_static_method("YAMLStyle", D_METHOD("string_style_string", "p_style"), &YAMLStyle::string_style_string);
	ClassDB::bind_static_method("YAMLStyle", D_METHOD("integer_format_string", "p_style"), &YAMLStyle::integer_format_string);
	ClassDB::bind_static_method("YAMLStyle", D_METHOD("float_format_string", "p_style"), &YAMLStyle::float_format_string);
	ClassDB::bind_static_method("YAMLStyle", D_METHOD("binary_encoding_string", "p_style"), &YAMLStyle::binary_encoding_string);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "container_form", PROPERTY_HINT_ENUM, "Any,Sequence,Mapping"), "set_container_form", "get_container_form");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "flow_style", PROPERTY_HINT_ENUM, "Any,None,Single"), "set_flow_style", "get_flow_style");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "string_style", PROPERTY_HINT_ENUM, "Any,Plain,Single Quoted,Double Quoted,Literal,Folded"), "set_string_style", "get_string_style");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "integer_format", PROPERTY_HINT_ENUM, "Any,Decimal,Hex,Octal,Binary,Scientific"), "set_integer_format", "get_integer_format");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "float_format", PROPERTY_HINT_ENUM, "Any,Decimal,Scientific"), "set_float_format", "get_float_format");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "binary_encoding", PROPERTY_HINT_ENUM, "Any,Base64,Hex"), "set_binary_encoding", "get_binary_encoding");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "custom_settings"), "set_custom_settings", "get_custom_settings");
}

Ref<YAMLStyle> YAMLStyle::set_container_form(ContainerForm p_style) {
	container_form = p_style;
	has_container_form = true;
	return Ref<YAMLStyle>(this);
}

YAMLStyle::ContainerForm YAMLStyle::get_container_form() const {
	return container_form;
}

Ref<YAMLStyle> YAMLStyle::set_flow_style(FlowStyle p_style) {
	flow_style = p_style;
	has_flow_style = true;
	return Ref<YAMLStyle>(this);
}

YAMLStyle::FlowStyle YAMLStyle::get_flow_style() const {
	return flow_style;
}

Ref<YAMLStyle> YAMLStyle::set_string_style(StringStyle p_style) {
	string_style = p_style;
	has_string_style = true;
	return Ref<YAMLStyle>(this);
}

YAMLStyle::StringStyle YAMLStyle::get_string_style() const {
	return string_style;
}

Ref<YAMLStyle> YAMLStyle::set_integer_format(IntegerFormat p_format) {
	integer_format = p_format;
	has_integer_format = true;
	return Ref<YAMLStyle>(this);
}

YAMLStyle::IntegerFormat YAMLStyle::get_integer_format() const {
	return integer_format;
}

Ref<YAMLStyle> YAMLStyle::set_float_format(FloatFormat p_format) {
	float_format = p_format;
	has_float_format = true;
	return Ref<YAMLStyle>(this);
}

YAMLStyle::FloatFormat YAMLStyle::get_float_format() const {
	return float_format;
}

Ref<YAMLStyle> YAMLStyle::set_binary_encoding(BinaryEncoding p_encoding) {
	binary_encoding = p_encoding;
	has_binary_encoding = true;
	return Ref<YAMLStyle>(this);
}

YAMLStyle::BinaryEncoding YAMLStyle::get_binary_encoding() const {
	return binary_encoding;
}

Ref<YAMLStyle> YAMLStyle::set_custom_settings(Dictionary p_custom) {
	custom_settings = p_custom;
	has_custom_settings = true;
	return Ref<YAMLStyle>(this);
}

Dictionary YAMLStyle::get_custom_settings() const {
	return custom_settings;
}

Ref<YAMLStyle> YAMLStyle::set_custom_tag(const String &p_tag) {
	custom_settings["tag"] = p_tag;
	return Ref<YAMLStyle>(this);
}

String YAMLStyle::get_custom_tag() const {
	Dictionary custom = get_custom_settings();
	if (custom.has("tag")) {
		return custom["tag"];
	}
	return "";
}

// Child style management implementations
Ref<YAMLStyle> YAMLStyle::get_child(const String &key) const {
	auto it = child_styles.find(key);
	return it != child_styles.end() ? it->second : Ref<YAMLStyle>();
}

Ref<YAMLStyle> YAMLStyle::set_child(const String &key, const Ref<YAMLStyle> &style) {
	if (style.is_valid()) {
		child_styles[key] = style;
	} else {
		child_styles.erase(key);
	}
	return Ref<YAMLStyle>(this);
}

bool YAMLStyle::has_child(const String &key) const {
	return child_styles.find(key) != child_styles.end();
}

Ref<YAMLStyle> YAMLStyle::clear_child(const String &key) {
	child_styles.erase(key);
	return Ref<YAMLStyle>(this);
}

Ref<YAMLStyle> YAMLStyle::clear_children() {
	child_styles.clear();
	return Ref<YAMLStyle>(this);
}

Array YAMLStyle::get_children_keys() const {
	Array keys;
	for (const auto &pair : child_styles) {
		keys.push_back(pair.first);
	}
	return keys;
}

uint32_t YAMLStyle::hash() const {
	uint32_t hash = hash_murmur3_one_32(container_form);
	hash = hash_murmur3_one_32(flow_style, hash);
	hash = hash_murmur3_one_32(string_style, hash);
	hash = hash_murmur3_one_32(integer_format, hash);
	hash = hash_murmur3_one_32(float_format, hash);
	hash = hash_murmur3_one_32(binary_encoding, hash);
	hash = hash_murmur3_one_32(custom_settings.hash(), hash);
	for (const auto &pair : child_styles) {
		hash = hash_murmur3_one_32(pair.first.hash(), hash);
		hash = hash_murmur3_one_32(pair.second->hash(), hash);
	}
	return hash;
}

void YAMLStyle::simplify() {
	if (container_form != FORM_SEQ) {
		return;
	}

	if (!has_child("_template")) {
		return;
	}

	Ref<YAMLStyle> template_style = child_styles["_template"];
	uint32_t template_hash = template_style->hash();

	Array child_keys = get_children_keys();
	if (child_keys.size() == 1) {
		return;
	}

	for (int i = 0; i < child_keys.size(); i++) {
		String key = child_keys[i];
		if (key == "_template") {
			continue;
		}

		Ref<YAMLStyle> child_style = child_styles[key];
		if (child_style->hash() == template_hash) {
			child_styles.erase(key);
		}
	}
}

void YAMLStyle::detect_string_style(const ryml::ConstNodeRef &node, const Ref<YAMLStyle> &style) {
	if (!node.has_val()) {
		return;
	}

	if (node.is_val_literal()) {
		style->set_string_style(YAMLStyle::STRING_LITERAL);
		return;
	} else if (node.is_val_folded()) {
		style->set_string_style(YAMLStyle::STRING_FOLDED);
		return;
	} else if (node.is_val_quoted()) {
		style->set_string_style(node.is_val_squo() ? YAMLStyle::STRING_QUOTE_SINGLE : YAMLStyle::STRING_QUOTE_DOUBLE);
		return;
	} else {
		style->set_string_style(YAMLStyle::STRING_PLAIN);
	}
}

void YAMLStyle::detect_flow_style(const ryml::ConstNodeRef &node, const Ref<YAMLStyle> &style) {
	if (node.is_flow()) {
		style->set_flow_style(YAMLStyle::FLOW_SINGLE);
	} else {
		style->set_flow_style(YAMLStyle::FLOW_NONE);
	}
}

String YAMLStyle::container_form_string(ContainerForm p_style) {
	switch (p_style) {
		case FORM_ANY:
			return "any";
		case FORM_SEQ:
			return "sequence";
		case FORM_MAP:
			return "map";
		default:
			return "any";
	}
}

String YAMLStyle::flow_style_string(FlowStyle p_style) {
	switch (p_style) {
		case FLOW_ANY:
			return "any";
		case FLOW_NONE:
			return "none";
		case FLOW_SINGLE:
			return "single";
		default:
			return "any";
	}
}

String YAMLStyle::string_style_string(StringStyle p_style) {
	switch (p_style) {
		case STRING_ANY:
			return "any";
		case STRING_PLAIN:
			return "plain";
		case STRING_QUOTE_SINGLE:
			return "quote_single";
		case STRING_QUOTE_DOUBLE:
			return "quote_double";
		case STRING_LITERAL:
			return "literal";
		case STRING_FOLDED:
			return "folded";
		default:
			return "any";
	}
}

String YAMLStyle::integer_format_string(IntegerFormat p_format) {
	switch (p_format) {
		case INT_ANY:
			return "any";
		case INT_DECIMAL:
			return "decimal";
		case INT_HEX:
			return "hex";
		case INT_OCTAL:
			return "octal";
		case INT_BINARY:
			return "binary";
		default:
			return "any";
	}
}

String YAMLStyle::float_format_string(FloatFormat p_format) {
	switch (p_format) {
		case FLOAT_ANY:
			return "any";
		case FLOAT_DECIMAL:
			return "decimal";
		case FLOAT_SCIENTIFIC:
			return "scientific";
		default:
			return "any";
	}
}

String YAMLStyle::binary_encoding_string(BinaryEncoding p_encoding) {
	switch (p_encoding) {
		case BIN_ANY:
			return "any";
		case BIN_BASE64:
			return "base64";
		case BIN_HEX:
			return "hex";
		default:
			return "any";
	}
}

String YAMLStyle::get_debug_string() const {
	String debug;
	debug += vformat("Container Form:  %s (%s)\n", container_form_string(container_form), has_container_form ? "Explicit" : "Inherited");
	debug += vformat("Flow Style:      %s (%s)\n", flow_style_string(flow_style), has_flow_style ? "Explicit" : "Inherited");
	debug += vformat("String Style:    %s (%s)\n", string_style_string(string_style), has_string_style ? "Explicit" : "Inherited");
	debug += vformat("Integer Format:  %s (%s)\n", integer_format_string(integer_format), has_integer_format ? "Explicit" : "Inherited");
	debug += vformat("Float Format:    %s (%s)\n", float_format_string(float_format), has_float_format ? "Explicit" : "Inherited");
	debug += vformat("Binary Encoding: %s (%s)\n", binary_encoding_string(binary_encoding), has_binary_encoding ? "Explicit" : "Inherited");

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

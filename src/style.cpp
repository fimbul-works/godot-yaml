#include "style.h"
#include "exception.h"
#include "result.h"
#include "style_view.h"
#include "yaml.h"

#include <godot_cpp/classes/file_access.hpp>
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

void YAMLStyle::_bind_methods() {
	// Bind enums
	BIND_ENUM_CONSTANT(FORM_ANY);
	BIND_ENUM_CONSTANT(FORM_ARRAY);
	BIND_ENUM_CONSTANT(FORM_DICTIONARY);

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

	ClassDB::bind_method(D_METHOD("clone"), &YAMLStyle::clone);
	ClassDB::bind_method(D_METHOD("merge_with", "other"), &YAMLStyle::merge_with);

	ClassDB::bind_method(D_METHOD("to_dictionary"), &YAMLStyle::to_dictionary);
	ClassDB::bind_method(D_METHOD("save_file", "path"), &YAMLStyle::save_file);
	ClassDB::bind_static_method("YAMLStyle", D_METHOD("from_dictionary", "dict"), &YAMLStyle::from_dictionary);
	ClassDB::bind_static_method("YAMLStyle", D_METHOD("load_file", "path"), &YAMLStyle::load_file);

	ClassDB::bind_method(D_METHOD("get_child", "key"), &YAMLStyle::get_child);
	ClassDB::bind_method(D_METHOD("set_child", "key", "style"), &YAMLStyle::set_child);
	ClassDB::bind_method(D_METHOD("has_child", "key"), &YAMLStyle::has_child);
	ClassDB::bind_method(D_METHOD("clear_child", "key"), &YAMLStyle::clear_child);
	ClassDB::bind_method(D_METHOD("clear_children"), &YAMLStyle::clear_children);
	ClassDB::bind_method(D_METHOD("get_children_keys"), &YAMLStyle::get_children_keys);

	ClassDB::bind_method(D_METHOD("is_block_style"), &YAMLStyle::is_block_style);
	ClassDB::bind_method(D_METHOD("uses_quotes"), &YAMLStyle::uses_quotes);
	ClassDB::bind_method(D_METHOD("uses_flow"), &YAMLStyle::uses_flow);

	ClassDB::bind_static_method("YAMLStyle", D_METHOD("container_form_string", "form"), &YAMLStyle::container_form_string);
	ClassDB::bind_static_method("YAMLStyle", D_METHOD("flow_style_string", "style"), &YAMLStyle::flow_style_string);
	ClassDB::bind_static_method("YAMLStyle", D_METHOD("string_style_string", "style"), &YAMLStyle::string_style_string);
	ClassDB::bind_static_method("YAMLStyle", D_METHOD("integer_format_string", "format"), &YAMLStyle::integer_format_string);
	ClassDB::bind_static_method("YAMLStyle", D_METHOD("float_format_string", "format"), &YAMLStyle::float_format_string);
	ClassDB::bind_static_method("YAMLStyle", D_METHOD("binary_encoding_string", "encoding"), &YAMLStyle::binary_encoding_string);

	ClassDB::bind_static_method("YAMLStyle", D_METHOD("container_form_from_string", "string"), &YAMLStyle::container_form_from_string);
	ClassDB::bind_static_method("YAMLStyle", D_METHOD("flow_style_from_string", "string"), &YAMLStyle::flow_style_from_string);
	ClassDB::bind_static_method("YAMLStyle", D_METHOD("string_style_from_string", "string"), &YAMLStyle::string_style_from_string);
	ClassDB::bind_static_method("YAMLStyle", D_METHOD("integer_format_from_string", "string"), &YAMLStyle::integer_format_from_string);
	ClassDB::bind_static_method("YAMLStyle", D_METHOD("float_format_from_string", "string"), &YAMLStyle::float_format_from_string);
	ClassDB::bind_static_method("YAMLStyle", D_METHOD("binary_encoding_from_string", "string"), &YAMLStyle::binary_encoding_from_string);

	ClassDB::bind_method(D_METHOD("get_debug_string"), &YAMLStyle::get_debug_string);

	BIND_VIRTUAL_METHOD(YAMLStyle, _to_string);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "container_form", PROPERTY_HINT_ENUM, "Any,Array,Dictionary"), "set_container_form", "get_container_form");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "flow_style", PROPERTY_HINT_ENUM, "Any,None,Single"), "set_flow_style", "get_flow_style");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "string_style", PROPERTY_HINT_ENUM, "Any,Plain,Single Quoted,Double Quoted,Literal,Folded"), "set_string_style", "get_string_style");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "integer_format", PROPERTY_HINT_ENUM, "Any,Decimal,Hex,Octal,Binary"), "set_integer_format", "get_integer_format");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "float_format", PROPERTY_HINT_ENUM, "Any,Decimal,Scientific"), "set_float_format", "get_float_format");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "binary_encoding", PROPERTY_HINT_ENUM, "Any,Base64,Hex"), "set_binary_encoding", "get_binary_encoding");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "custom_settings"), "set_custom_settings", "get_custom_settings");
}

Ref<YAMLStyle> YAMLStyle::clone() const {
	Ref<YAMLStyle> clone;
	clone.instantiate();

	clone->has_container_form = has_container_form;
	clone->container_form = container_form;

	clone->has_flow_style = has_flow_style;
	clone->flow_style = flow_style;

	clone->has_string_style = has_string_style;
	clone->string_style = string_style;

	clone->has_integer_format = has_integer_format;
	clone->integer_format = integer_format;

	clone->has_float_format = has_float_format;
	clone->float_format = float_format;

	clone->has_binary_encoding = has_binary_encoding;
	clone->binary_encoding = binary_encoding;

	clone->custom_settings = custom_settings.duplicate(true);

	for (const auto &pair : child_styles) {
		clone->set_child(pair.first, pair.second->clone());
	}

	return clone;
}

Ref<YAMLStyle> YAMLStyle::merge_with(const Ref<YAMLStyle> &other) {
	if (!other.is_valid()) {
		return Ref<YAMLStyle>(this);
	}

	if (other->has_container_form) {
		set_container_form(other->container_form);
	}

	if (other->has_flow_style) {
		set_flow_style(other->flow_style);
	}

	if (other->has_string_style) {
		set_string_style(other->string_style);
	}

	if (other->has_integer_format) {
		set_integer_format(other->integer_format);
	}

	if (other->has_float_format) {
		set_float_format(other->float_format);
	}

	if (other->has_binary_encoding) {
		set_binary_encoding(other->binary_encoding);
	}

	custom_settings.merge(other->custom_settings, true);

	Array other_keys = other->get_children_keys();
	for (int i = 0; i < other_keys.size(); i++) {
		String key = other_keys[i];
		Ref<YAMLStyle> other_child = other->get_child(key);

		if (has_child(key)) {
			Ref<YAMLStyle> my_child = get_child(key);
			my_child->merge_with(other_child);
		} else {
			set_child(key, other_child->clone());
		}
	}

	return Ref<YAMLStyle>(this);
}

Dictionary YAMLStyle::to_dictionary() const {
	Dictionary dict;

	if (has_container_form) {
		dict["form"] = container_form_string(container_form);
	}

	if (has_flow_style) {
		dict["flow"] = flow_style_string(flow_style);
	}

	if (has_string_style) {
		dict["string"] = string_style_string(string_style);
	}

	if (has_integer_format) {
		dict["integer"] = integer_format_string(integer_format);
	}

	if (has_float_format) {
		dict["float"] = float_format_string(float_format);
	}

	if (has_binary_encoding) {
		dict["binary"] = binary_encoding_string(binary_encoding);
	}

	if (!custom_settings.is_empty()) {
		dict["custom_settings"] = custom_settings.duplicate(true);
	}

	Array keys = get_children_keys();
	if (keys.size() > 0) {
		Dictionary children_dict;
		for (size_t i = 0; i < keys.size(); i++) {
			String key = keys[i];
			Ref<YAMLStyle> child = get_child(key);
			children_dict[key] = child->to_dictionary();
		}
		dict["children"] = children_dict;
	}

	return dict;
}

Ref<YAMLStyle> YAMLStyle::from_dictionary(const Dictionary &dict) {
	Ref<YAMLStyle> style;
	style.instantiate();

	if (dict.has("form")) {
		style->set_container_form(container_form_from_string(String(dict["form"])));
	}

	if (dict.has("flow")) {
		style->set_flow_style(flow_style_from_string(String(dict["flow"])));
	}

	if (dict.has("string")) {
		style->set_string_style(string_style_from_string(String(dict["string"])));
	}

	if (dict.has("integer")) {
		style->set_integer_format(integer_format_from_string(String(dict["integer"])));
	}

	if (dict.has("float")) {
		style->set_float_format(float_format_from_string(String(dict["float"])));
	}

	if (dict.has("binary")) {
		style->set_binary_encoding(binary_encoding_from_string(String(dict["binary"])));
	}

	if (dict.has("custom_settings") && dict["custom_settings"].get_type() == Variant::DICTIONARY) {
		style->set_custom_settings(dict["custom_settings"].duplicate(true));
	}

	if (dict.has("children") && dict["children"].get_type() == Variant::DICTIONARY) {
		Dictionary children = dict["children"];
		Array keys = children.keys();
		for (size_t i = 0; i < keys.size(); i++) {
			String key = keys[i];
			if (children[key].get_type() != Variant::DICTIONARY) {
				continue;
			}
			style->set_child(key, from_dictionary(children[key]));
		}
	}

	return style;
}

Ref<YAMLResult> YAMLStyle::save_file(const String &path) {
	Ref<YAMLResult> result = YAML::save_file(to_dictionary(), path);
	if (result->has_error()) {
		return result;
	}

	return YAMLResult::success(Variant());
}

Ref<YAMLResult> YAMLStyle::load_file(const String &path) {
	Ref<YAMLResult> result = YAML::load_file(path);
	if (result->has_error()) {
		return result;
	}

	return YAMLResult::success(from_dictionary(result->get_data()));
}

Ref<YAMLStyle> YAMLStyle::set_container_form(ContainerForm form) {
	container_form = form;
	has_container_form = true;
	return Ref<YAMLStyle>(this);
}

YAMLStyle::ContainerForm YAMLStyle::get_container_form() const {
	return container_form;
}

Ref<YAMLStyle> YAMLStyle::set_flow_style(FlowStyle style) {
	flow_style = style;
	has_flow_style = true;
	return Ref<YAMLStyle>(this);
}

YAMLStyle::FlowStyle YAMLStyle::get_flow_style() const {
	return flow_style;
}

Ref<YAMLStyle> YAMLStyle::set_string_style(StringStyle style) {
	string_style = style;
	has_string_style = true;
	return Ref<YAMLStyle>(this);
}

YAMLStyle::StringStyle YAMLStyle::get_string_style() const {
	return string_style;
}

Ref<YAMLStyle> YAMLStyle::set_integer_format(IntegerFormat format) {
	integer_format = format;
	has_integer_format = true;
	return Ref<YAMLStyle>(this);
}

YAMLStyle::IntegerFormat YAMLStyle::get_integer_format() const {
	return integer_format;
}

Ref<YAMLStyle> YAMLStyle::set_float_format(FloatFormat format) {
	float_format = format;
	has_float_format = true;
	return Ref<YAMLStyle>(this);
}

YAMLStyle::FloatFormat YAMLStyle::get_float_format() const {
	return float_format;
}

Ref<YAMLStyle> YAMLStyle::set_binary_encoding(BinaryEncoding encoding) {
	binary_encoding = encoding;
	has_binary_encoding = true;
	return Ref<YAMLStyle>(this);
}

YAMLStyle::BinaryEncoding YAMLStyle::get_binary_encoding() const {
	return binary_encoding;
}

Ref<YAMLStyle> YAMLStyle::set_custom_settings(Dictionary settings) {
	custom_settings = settings;
	has_custom_settings = true;
	return Ref<YAMLStyle>(this);
}

Dictionary YAMLStyle::get_custom_settings() const {
	return custom_settings;
}

Ref<YAMLStyle> YAMLStyle::set_custom_setting(const String &key, const Variant &value) {
	get_custom_settings()[key] = value;
	return Ref<YAMLStyle>(this);
}

Variant YAMLStyle::get_custom_setting(const String &key) const {
	return get_custom_settings()[key];
}

Ref<YAMLStyle> YAMLStyle::set_custom_tag(const String &tag) {
	set_custom_setting("tag", tag);
	return Ref<YAMLStyle>(this);
}

String YAMLStyle::get_custom_tag() const {
	if (custom_settings.has("tag")) {
		return get_custom_setting("tag");
	}
	return "";
}

bool YAMLStyle::has_child(const String &key) const {
	return child_styles.find(key) != child_styles.end();
}

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

String YAMLStyle::container_form_string(ContainerForm form) {
	switch (form) {
		case FORM_ANY:
			return "any";
		case FORM_ARRAY:
			return "array";
		case FORM_DICTIONARY:
			return "dictionary";
		default:
			return "any";
	}
}

YAMLStyle::ContainerForm YAMLStyle::container_form_from_string(const String &string) {
	static std::unordered_map<String, ContainerForm, StringHasher, StringEqual> form_map;
	static std::once_flag init_flag;

	std::call_once(init_flag, []() {
		form_map["any"] = FORM_ANY;
		form_map["array"] = FORM_ARRAY;
		form_map["dictionary"] = FORM_DICTIONARY;
	});

	auto it = form_map.find(string);
	return it != form_map.end() ? it->second : FORM_ANY;
}

String YAMLStyle::flow_style_string(FlowStyle style) {
	switch (style) {
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

YAMLStyle::FlowStyle YAMLStyle::flow_style_from_string(const String &string) {
	static std::unordered_map<String, FlowStyle, StringHasher, StringEqual> flow_map;
	static std::once_flag init_flag;

	std::call_once(init_flag, []() {
		flow_map["any"] = FLOW_ANY;
		flow_map["none"] = FLOW_NONE;
		flow_map["single"] = FLOW_SINGLE;
	});

	auto it = flow_map.find(string);
	return it != flow_map.end() ? it->second : FLOW_ANY;
}

String YAMLStyle::string_style_string(StringStyle style) {
	switch (style) {
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

YAMLStyle::StringStyle YAMLStyle::string_style_from_string(const String &string) {
	static std::unordered_map<String, StringStyle, StringHasher, StringEqual> string_map;
	static std::once_flag init_flag;

	std::call_once(init_flag, []() {
		string_map["any"] = STRING_ANY;
		string_map["plain"] = STRING_PLAIN;
		string_map["quote_single"] = STRING_QUOTE_SINGLE;
		string_map["quote_double"] = STRING_QUOTE_DOUBLE;
		string_map["literal"] = STRING_LITERAL;
		string_map["folded"] = STRING_FOLDED;
	});

	auto it = string_map.find(string);
	return it != string_map.end() ? it->second : STRING_ANY;
}

String YAMLStyle::integer_format_string(IntegerFormat format) {
	switch (format) {
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

YAMLStyle::IntegerFormat YAMLStyle::integer_format_from_string(const String &string) {
	static std::unordered_map<String, IntegerFormat, StringHasher, StringEqual> int_map;
	static std::once_flag init_flag;

	std::call_once(init_flag, []() {
		int_map["any"] = INT_ANY;
		int_map["decimal"] = INT_DECIMAL;
		int_map["hex"] = INT_HEX;
		int_map["octal"] = INT_OCTAL;
		int_map["binary"] = INT_BINARY;
	});

	auto it = int_map.find(string);
	return it != int_map.end() ? it->second : INT_ANY;
}

String YAMLStyle::float_format_string(FloatFormat format) {
	switch (format) {
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

YAMLStyle::FloatFormat YAMLStyle::float_format_from_string(const String &string) {
	static std::unordered_map<String, FloatFormat, StringHasher, StringEqual> float_map;
	static std::once_flag init_flag;

	std::call_once(init_flag, []() {
		float_map["any"] = FLOAT_ANY;
		float_map["decimal"] = FLOAT_DECIMAL;
		float_map["scientific"] = FLOAT_SCIENTIFIC;
	});

	auto it = float_map.find(string);
	return it != float_map.end() ? it->second : FLOAT_ANY;
}

String YAMLStyle::binary_encoding_string(BinaryEncoding encoding) {
	switch (encoding) {
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

YAMLStyle::BinaryEncoding YAMLStyle::binary_encoding_from_string(const String &string) {
	static std::unordered_map<String, BinaryEncoding, StringHasher, StringEqual> binary_map;
	static std::once_flag init_flag;

	std::call_once(init_flag, []() {
		binary_map["any"] = BIN_ANY;
		binary_map["base64"] = BIN_BASE64;
		binary_map["hex"] = BIN_HEX;
	});

	auto it = binary_map.find(string);
	return it != binary_map.end() ? it->second : BIN_ANY;
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
	if (container_form != FORM_ARRAY) {
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

bool YAMLStyle::is_block_style() const {
	return string_style == STRING_LITERAL || string_style == STRING_FOLDED;
}

bool YAMLStyle::uses_quotes() const {
	return string_style == STRING_QUOTE_SINGLE || string_style == STRING_QUOTE_DOUBLE;
}

bool YAMLStyle::uses_flow() const {
	return flow_style == FLOW_SINGLE;
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

String YAMLStyle::_to_string() const {
	return vformat("YAMLStyle(%d)", hash());
}

String YAMLStyle::get_debug_string() const {
	String debug;

	if (has_container_form) {
		debug += vformat("Form: %s\n", container_form_string(container_form));
	}

	if (has_flow_style) {
		debug += vformat("Flow: %s\n", flow_style_string(flow_style));
	}

	if (has_string_style) {
		debug += vformat("String: %s\n", string_style_string(string_style));
	}

	if (has_integer_format) {
		debug += vformat("Integer: %s\n", integer_format_string(integer_format));
	}

	if (has_float_format) {
		debug += vformat("Float: %s\n", float_format_string(float_format));
	}

	if (has_binary_encoding) {
		debug += vformat("Binary: %s\n", binary_encoding_string(binary_encoding));
	}

	if (!custom_settings.is_empty()) {
		debug += "\nSettings:\n";
		Array keys = custom_settings.keys();
		for (int i = 0; i < keys.size(); i++) {
			debug += vformat("  %s: %s\n", String(keys[i]), String(custom_settings[keys[i]]));
		}
	}

	if (!child_styles.empty()) {
		debug += "\nChildren:\n";
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

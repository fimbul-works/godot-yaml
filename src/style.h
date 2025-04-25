#ifndef YAML_STYLE_H
#define YAML_STYLE_H

#include "string_hash.h"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <ryml.hpp>

#include <memory>
#include <unordered_map>

namespace godot {

// Forward declarations
class YAMLStyle : public RefCounted {
	GDCLASS(YAMLStyle, RefCounted);

public:
	class View;

	enum ContainerForm {
		FORM_ANY, // Let emitter decide
		FORM_SEQ, // List/array style
		FORM_MAP // Dictionary/map style
	};

	enum FlowStyle {
		FLOW_ANY, // Let emitter decide
		FLOW_NONE, // Block style
		FLOW_SINGLE, // Compact [a,b] or {k:v} style
	};

	enum StringStyle {
		STRING_ANY, // Let emitter decide
		STRING_PLAIN, // Regular unquoted string
		STRING_QUOTE_SINGLE, // 'string'
		STRING_QUOTE_DOUBLE, // "string"
		STRING_LITERAL, // | string
		STRING_FOLDED // > string
	};

	enum IntegerFormat {
		INT_ANY, // Let emitter decide
		INT_DECIMAL, // Standard decimal
		INT_HEX, // Hexadecimal (0xFF)
		INT_OCTAL, // Octal (0o700)
		INT_BINARY, // Binary (0b1010)
		INT_SCIENTIFIC // Scientific (1.23e+4)
	};

	enum FloatFormat {
		FLOAT_ANY, // Let emitter decide
		FLOAT_DECIMAL, // Standard decimal
		FLOAT_SCIENTIFIC // Scientific (1.23e+4)
	};

	enum BinaryEncoding {
		BIN_ANY, // Let emitter decide
		BIN_BASE64, // Base64 encoded
		BIN_HEX // Hexadecimal encoded
	};

	YAMLStyle();

	// Style setters/getters
	bool has_container_form = false;
	void set_container_form(ContainerForm p_style) {
		container_form = p_style;
		has_container_form = p_style != FORM_ANY;
	}
	ContainerForm get_container_form() const { return container_form; }

	bool has_flow_style = false;
	void set_flow_style(FlowStyle p_style) {
		flow_style = p_style;
		has_flow_style = p_style != FLOW_ANY;
	}
	FlowStyle get_flow_style() const { return flow_style; }

	bool has_string_style = false;
	void set_string_style(StringStyle p_style) {
		string_style = p_style;
		has_string_style = p_style != STRING_ANY;
	}
	StringStyle get_string_style() const { return string_style; }

	bool has_integer_format = false;
	void set_integer_format(IntegerFormat p_format) {
		integer_format = p_format;
		has_integer_format = p_format != INT_ANY;
	}
	IntegerFormat get_integer_format() const { return integer_format; }

	bool has_float_format = false;
	void set_float_format(FloatFormat p_format) {
		float_format = p_format;
		has_float_format = p_format != FLOAT_ANY;
	}
	FloatFormat get_float_format() const { return float_format; }

	bool has_binary_encoding = false;
	void set_binary_encoding(BinaryEncoding p_encoding) {
		binary_encoding = p_encoding;
		has_binary_encoding = p_encoding != BIN_ANY;
	}
	BinaryEncoding get_binary_encoding() const { return binary_encoding; }

	bool has_custom_settings = false;
	void set_custom_settings(Dictionary p_custom) {
		custom_settings = p_custom;
		has_custom_settings = true;
	}
	Dictionary get_custom_settings() const { return custom_settings; }

	void set_custom_tag(const String &p_tag) {
		custom_settings["tag"] = p_tag;
	}
	String get_custom_tag() const {
		Dictionary custom = get_custom_settings();
		if (custom.has("tag")) {
			return custom["tag"];
		}
		return "";
	}

	// Helper methods
	bool is_block_style() const;
	bool uses_quotes() const;
	bool uses_flow() const;

	// Child style management
	Ref<YAMLStyle> get_child(const String &key) const;
	void set_child(const String &key, const Ref<YAMLStyle> &style);
	bool has_child(const String &key) const;
	void clear_child(const String &key);
	void clear_children();
	Array get_children_keys() const;

	// Debug helper
	String get_debug_string() const;

	// Custom settings
	Dictionary custom_settings;

protected:
	static void _bind_methods();

private:
	ContainerForm container_form;
	FlowStyle flow_style;
	StringStyle string_style;
	IntegerFormat integer_format;
	FloatFormat float_format;
	BinaryEncoding binary_encoding;

	std::unordered_map<String, Ref<YAMLStyle>, StringHasher, StringEqual> child_styles;

	static String get_container_form_string(ContainerForm p_style);
	static String get_flow_style_string(FlowStyle p_style);
	static String get_string_style_string(StringStyle p_style);
	static String get_integer_format_string(IntegerFormat p_format);
	static String get_float_format_string(FloatFormat p_format);
	static String get_binary_encoding_string(BinaryEncoding p_encoding);
};

} // namespace godot

VARIANT_ENUM_CAST(YAMLStyle::ContainerForm);
VARIANT_ENUM_CAST(YAMLStyle::FlowStyle);
VARIANT_ENUM_CAST(YAMLStyle::StringStyle);
VARIANT_ENUM_CAST(YAMLStyle::IntegerFormat);
VARIANT_ENUM_CAST(YAMLStyle::FloatFormat);
VARIANT_ENUM_CAST(YAMLStyle::BinaryEncoding);

#endif // YAML_STYLE_H

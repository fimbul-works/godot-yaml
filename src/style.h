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
	Ref<YAMLStyle> set_container_form(ContainerForm p_style);
	ContainerForm get_container_form() const;

	Ref<YAMLStyle> set_flow_style(FlowStyle p_style);
	FlowStyle get_flow_style() const;

	Ref<YAMLStyle> set_string_style(StringStyle p_style);
	StringStyle get_string_style() const;

	Ref<YAMLStyle> set_integer_format(IntegerFormat p_format);
	IntegerFormat get_integer_format() const;

	Ref<YAMLStyle> set_float_format(FloatFormat p_format);
	FloatFormat get_float_format() const;

	Ref<YAMLStyle> set_binary_encoding(BinaryEncoding p_encoding);
	BinaryEncoding get_binary_encoding() const;

	Ref<YAMLStyle> set_custom_settings(Dictionary p_custom);
	Dictionary get_custom_settings() const;

	Ref<YAMLStyle> set_custom_tag(const String &p_tag);
	String get_custom_tag() const;

	// Style detection helpers
	static void detect_flow_style(const ryml::ConstNodeRef &node, const Ref<YAMLStyle> &style);
	static void detect_string_style(const ryml::ConstNodeRef &node, const Ref<YAMLStyle> &style);

	// Helper methods
	bool is_block_style() const;
	bool uses_quotes() const;
	bool uses_flow() const;

	// Style simplification
	uint32_t hash() const;
	void simplify();

	// Child style management
	Ref<YAMLStyle> get_child(const String &key) const;
	Ref<YAMLStyle> set_child(const String &key, const Ref<YAMLStyle> &style);
	bool has_child(const String &key) const;
	Ref<YAMLStyle> clear_child(const String &key);
	Ref<YAMLStyle> clear_children();
	Array get_children_keys() const;

	// Debug helper
	String get_debug_string() const;
	static String container_form_string(ContainerForm p_style);
	static String flow_style_string(FlowStyle p_style);
	static String string_style_string(StringStyle p_style);
	static String integer_format_string(IntegerFormat p_format);
	static String float_format_string(FloatFormat p_format);
	static String binary_encoding_string(BinaryEncoding p_encoding);

	// Custom settings
	Dictionary custom_settings;

protected:
	static void _bind_methods();

private:
	ContainerForm container_form = FORM_ANY;
	FlowStyle flow_style = FLOW_ANY;
	StringStyle string_style = STRING_ANY;
	IntegerFormat integer_format = INT_ANY;
	FloatFormat float_format = FLOAT_ANY;
	BinaryEncoding binary_encoding = BIN_ANY;

	bool has_container_form = false;
	bool has_flow_style = false;
	bool has_string_style = false;
	bool has_integer_format = false;
	bool has_float_format = false;
	bool has_binary_encoding = false;
	bool has_custom_settings = false;

	std::unordered_map<String, Ref<YAMLStyle>, StringHasher, StringEqual> child_styles;
};

} // namespace godot

VARIANT_ENUM_CAST(YAMLStyle::ContainerForm);
VARIANT_ENUM_CAST(YAMLStyle::FlowStyle);
VARIANT_ENUM_CAST(YAMLStyle::StringStyle);
VARIANT_ENUM_CAST(YAMLStyle::IntegerFormat);
VARIANT_ENUM_CAST(YAMLStyle::FloatFormat);
VARIANT_ENUM_CAST(YAMLStyle::BinaryEncoding);

#endif // YAML_STYLE_H

/**
 * @file style.h
 * @brief Defines the YAMLStyle class for YAML formatting customization.
 *
 * This file contains the YAMLStyle class which controls the formatting and style
 * of YAML output. It provides options for customizing container formats, flow styles,
 * string representations, numeric formats, and more.
 */

#ifndef YAML_STYLE_H
#define YAML_STYLE_H

#include "result.h"
#include "string_hash.h"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <ryml.hpp>

#include <memory>
#include <unordered_map>

namespace godot {

// Forward reference
class YAMLResult;

/**
 * @class YAMLStyle
 * @brief Controls formatting and style options for YAML output.
 *
 * The YAMLStyle class provides extensive customization options for YAML
 * formatting, allowing control over:
 * - Container formats (array vs. dictionary)
 * - Flow styles (block vs. inline)
 * - String styles (plain, quoted, literal, folded)
 * - Numeric formats (decimal, hex, octal, binary)
 * - Float formats (decimal, scientific)
 * - Binary encoding (base64 vs. hex)
 *
 * It supports a hierarchical style structure where different parts of a YAML
 * document can have different formatting options.
 *
 * @extends RefCounted
 */
class YAMLStyle : public RefCounted {
	GDCLASS(YAMLStyle, RefCounted);

public:
	/**
	 * @class View
	 * @brief Forward declaration of the View class defined in style_view.h.
	 */
	class View;

	/**
	 * @enum ContainerForm
	 * @brief Specifies the container format for collections.
	 */
	enum ContainerForm {
		FORM_ANY, ///< Let emitter decide
		FORM_ARRAY, ///< Sequence/Array style
		FORM_DICTIONARY ///< Map/Dictionary style
	};

	/**
	 * @enum FlowStyle
	 * @brief Controls the flow style (block vs. inline).
	 */
	enum FlowStyle {
		FLOW_ANY, ///< Let emitter decide
		FLOW_NONE, ///< Block style
		FLOW_SINGLE, ///< Compact [a,b] or {k:v} style
	};

	/**
	 * @enum StringStyle
	 * @brief Determines how strings are formatted.
	 */
	enum StringStyle {
		STRING_ANY, ///< Let emitter decide
		STRING_PLAIN, ///< Regular unquoted string
		STRING_QUOTE_SINGLE, ///< 'string'
		STRING_QUOTE_DOUBLE, ///< "string"
		STRING_LITERAL, ///< | string (preserves newlines)
		STRING_FOLDED ///< > string (folds newlines)
	};

	/**
	 * @enum IntegerFormat
	 * @brief Specifies the format for integer values.
	 */
	enum IntegerFormat {
		INT_ANY, ///< Let emitter decide
		INT_DECIMAL, ///< Standard decimal
		INT_HEX, ///< Hexadecimal (0xFF)
		INT_OCTAL, ///< Octal (0o700)
		INT_BINARY, ///< Binary (0b1010)
	};

	/**
	 * @enum FloatFormat
	 * @brief Controls the format for floating-point values.
	 */
	enum FloatFormat {
		FLOAT_ANY, ///< Let emitter decide
		FLOAT_DECIMAL, ///< Standard decimal
		FLOAT_SCIENTIFIC ///< Scientific (1.23e+4)
	};

	/**
	 * @brief Default constructor.
	 *
	 * Initializes a style with default "ANY" values for all style options.
	 */
	YAMLStyle();

	/**
	 * @brief Clone and merge operations.
	 */

	/**
	 * @brief Creates a deep copy of this style.
	 *
	 * @return Ref<YAMLStyle> A new style object with the same settings
	 */
	Ref<YAMLStyle> clone() const;

	/**
	 * @brief Merges another style into this one.
	 *
	 * Values from the other style override this style's values
	 * when they are explicitly set.
	 *
	 * @param other The style to merge from
	 * @return Ref<YAMLStyle> This style object for chaining
	 */
	Ref<YAMLStyle> merge_with(const Ref<YAMLStyle> &other);

	/**
	 * @brief Serialization/Deserialization
	 */

	/**
	 * @brief Serializes the YAMLStyle into a Dictionary.
	 *
	 * @return Dictionary The serialized YAMLStyle
	 */
	Dictionary to_dictionary() const;

	/**
	 * @brief Deserializes a Dictionary into YAMLStyle.
	 *
	 * @param dict The dictionary containing style definitions
	 * @return Ref<YAMLStyle> Deserialized YAMLStyle
	 */
	static Ref<YAMLStyle> from_dictionary(const Dictionary &dict);

	/**
	 * @brief Serializes and saves a YAMLStyle file.
	 *
	 * @param path Path where the YAML file will be saved.
	 * @return Ref<YAMLResult> Result object indicating success or failure.
	 */
	Ref<YAMLResult> save_file(const String &path);

	/**
	 * @brief Loads and parses a YAMLStyle file.
	 *
	 * @param path Path to the YAML file.
	 * @return Ref<YAMLResult> Result object containing parsed YAMLStyle or error information.
	 */
	static Ref<YAMLResult> load_file(const String &path);

	/**
	 * @brief Style setters/getters.
	 */

	/**
	 * @brief Sets the container form.
	 *
	 * @param form The container form
	 * @return Ref<YAMLStyle> This style object for chaining
	 */
	Ref<YAMLStyle> set_container_form(ContainerForm form);

	/**
	 * @brief Gets the container form.
	 *
	 * @return ContainerForm The current container form
	 */
	ContainerForm get_container_form() const;

	/**
	 * @brief Sets the flow style.
	 *
	 * @param style The flow style
	 * @return Ref<YAMLStyle> This style object for chaining
	 */
	Ref<YAMLStyle> set_flow_style(FlowStyle style);

	/**
	 * @brief Gets the flow style.
	 *
	 * @return FlowStyle The current flow style
	 */
	FlowStyle get_flow_style() const;

	/**
	 * @brief Sets the string style.
	 *
	 * @param style The string style
	 * @return Ref<YAMLStyle> This style object for chaining
	 */
	Ref<YAMLStyle> set_string_style(StringStyle style);

	/**
	 * @brief Gets the string style.
	 *
	 * @return StringStyle The current string style
	 */
	StringStyle get_string_style() const;

	/**
	 * @brief Sets the integer format.
	 *
	 * @param format The integer format
	 * @return Ref<YAMLStyle> This style object for chaining
	 */
	Ref<YAMLStyle> set_integer_format(IntegerFormat format);

	/**
	 * @brief Gets the integer format.
	 *
	 * @return IntegerFormat The current integer format
	 */
	IntegerFormat get_integer_format() const;

	/**
	 * @brief Sets the float format.
	 *
	 * @param format The float format
	 * @return Ref<YAMLStyle> This style object for chaining
	 */
	Ref<YAMLStyle> set_float_format(FloatFormat format);

	/**
	 * @brief Gets the float format.
	 *
	 * @return FloatFormat The current float format
	 */
	FloatFormat get_float_format() const;

	/**
	 * @brief Sets custom settings as a Dictionary.
	 *
	 * @param settings The custom settings
	 * @return Ref<YAMLStyle> This style object for chaining
	 */
	Ref<YAMLStyle> set_custom_settings(Dictionary settings);

	/**
	 * @brief Gets the custom settings.
	 *
	 * @return Dictionary The current custom settings
	 */
	Dictionary get_custom_settings() const;

	/**
	 * @brief Sets a single custom setting.
	 *
	 * @param key The setting key
	 * @param value The setting value
	 * @return Ref<YAMLStyle> This style object for chaining
	 */
	Ref<YAMLStyle> set_custom_setting(const String &key, const Variant &value);

	/**
	 * @brief Gets a single custom setting.
	 *
	 * @param key The setting key
	 * @return Variant The setting value
	 */
	Variant get_custom_setting(const String &key) const;

	/**
	 * @brief Sets a custom tag for the current node.
	 *
	 * @param p_tag The tag value (without '!')
	 * @return Ref<YAMLStyle> This style object for chaining
	 */
	Ref<YAMLStyle> set_custom_tag(const String &tag);

	/**
	 * @brief Gets the custom tag.
	 *
	 * @return String The custom tag or empty string if not set
	 */
	String get_custom_tag() const;

	/**
	 * @brief Style detection helpers.
	 */

	/**
	 * @brief Detects flow style from a YAML node and updates the style.
	 *
	 * @param node The YAML node to analyze
	 * @param style The style object to update
	 */
	static void detect_flow_style(const ryml::ConstNodeRef &node, const Ref<YAMLStyle> &style);

	/**
	 * @brief Detects string style from a YAML node and updates the style.
	 *
	 * @param node The YAML node to analyze
	 * @param style The style object to update
	 */
	static void detect_string_style(const ryml::ConstNodeRef &node, const Ref<YAMLStyle> &style);

	/**
	 * @brief Helper methods for style properties.
	 */

	/**
	 * @brief Checks if using block string style.
	 *
	 * @return bool True if using literal or folded style
	 */
	bool is_block_style() const;

	/**
	 * @brief Checks if using quoted string style.
	 *
	 * @return bool True if using single or double quotes
	 */
	bool uses_quotes() const;

	/**
	 * @brief Checks if using flow (inline) style.
	 *
	 * @return bool True if using flow style
	 */
	bool uses_flow() const;

	/**
	 * @brief Style simplification methods.
	 */

	/**
	 * @brief Computes a hash value for this style.
	 *
	 * Useful for comparing styles efficiently.
	 *
	 * @return uint32_t The hash value
	 */
	uint32_t hash() const;

	/**
	 * @brief Simplifies the style structure.
	 *
	 * Removes redundant child styles that match a template.
	 */
	void simplify();

	/**
	 * @brief Child style management.
	 */

	/**
	 * @brief Creates a new child style if it doesn't exist, or returns the existing one.
	 *
	 * @param key The child key
	 * @param inherit_styles Should the child inherits all scalar styles, like number formats and string styles
	 * @return Ref<YAMLStyle> The new or existing child style
	 */
	Ref<YAMLStyle> create_child(const String &key, const bool inherit_styles = true);

	/**
	 * @brief Checks if a child style exists for a key.
	 *
	 * @param key The child key
	 * @return bool True if the child exists
	 */
	bool has_child(const String &key) const;

	/**
	 * @brief Gets a child style.
	 *
	 * @param key The child key
	 * @return Ref<YAMLStyle> The child style or null if not found
	 */
	Ref<YAMLStyle> get_child(const String &key) const;

	/**
	 * @brief Sets a child style.
	 *
	 * @param key The child key
	 * @param style The child style
	 * @return Ref<YAMLStyle> This style object for chaining
	 */
	Ref<YAMLStyle> set_child(const String &key, const Ref<YAMLStyle> &style);

	/**
	 * @brief Gets a child style at the specified path, optionally creating missing nodes.
	 *
	 * The path uses '/' as a separator. Array indices can be specified numerically.
	 * Example: "maps/items/0/properties"
	 *
	 * @param path The path to the child style
	 * @param create_if_missing Whether to create missing children along the path
	 * @return Ref<YAMLStyle> The child style or null if not found (and create_if_missing is false)
	 */
	Ref<YAMLStyle> get_at_path(const String &path, bool create_if_missing = false);

	/**
	 * @brief Removes a child style.
	 *
	 * @param key The child key
	 * @return Ref<YAMLStyle> This style object for chaining
	 */
	Ref<YAMLStyle> clear_child(const String &key);

	/**
	 * @brief Removes all child styles.
	 *
	 * @return Ref<YAMLStyle> This style object for chaining
	 */
	Ref<YAMLStyle> clear_children();

	/**
	 * @brief Gets the keys of all child styles.
	 *
	 * @return Array Array of child keys
	 */
	Array list_children() const;

	/**
	 * @brief Copies all scalar styles (string style, number format, binary encoding) to the target.
	 *
	 * @param target The recipient of the styles
	 * @return Ref<YAMLStyle> This style object for chaining
	 */
	Ref<YAMLStyle> propagate_scalar_styles(Ref<YAMLStyle> target) const;

	/**
	 * @brief Copies all scalar styles (string style, number format, binary encoding) to all child styles.
	 *
	 * @return Ref<YAMLStyle> This style object for chaining
	 */
	Ref<YAMLStyle> apply_scalar_styles_to_children() const;

	/**
	 * @brief Debug helper.
	 *
	 * @return String Detailed debug representation of the style
	 */
	String get_debug_string() const;

	/**
	 * @brief Converts to a string representation.
	 *
	 * @return String String representation of the style
	 */
	String _to_string() const;

	/**
	 * @brief Utility methods for enum conversions.
	 */

	/**
	 * @brief Converts ContainerForm to string.
	 *
	 * @param form The container form
	 * @return String String representation
	 */
	static String container_form_string(ContainerForm form);

	/**
	 * @brief Converts FlowStyle to string.
	 *
	 * @param style The flow style
	 * @return String String representation
	 */
	static String flow_style_string(FlowStyle style);

	/**
	 * @brief Converts StringStyle to string.
	 *
	 * @param style The string style
	 * @return String String representation
	 */
	static String string_style_string(StringStyle style);

	/**
	 * @brief Converts IntegerFormat to string.
	 *
	 * @param format The integer format
	 * @return String String representation
	 */
	static String integer_format_string(IntegerFormat format);

	/**
	 * @brief Converts FloatFormat to string.
	 *
	 * @param format The float format
	 * @return String String representation
	 */
	static String float_format_string(FloatFormat format);

	/**
	 * @brief Parses string to ContainerForm.
	 *
	 * @param string The string to parse
	 * @return ContainerForm The parsed container form
	 */
	static ContainerForm container_form_from_string(const String &string);

	/**
	 * @brief Parses string to FlowStyle.
	 *
	 * @param string The string to parse
	 * @return FlowStyle The parsed flow style
	 */
	static FlowStyle flow_style_from_string(const String &string);

	/**
	 * @brief Parses string to StringStyle.
	 *
	 * @param string The string to parse
	 * @return StringStyle The parsed string style
	 */
	static StringStyle string_style_from_string(const String &string);

	/**
	 * @brief Parses string to IntegerFormat.
	 *
	 * @param string The string to parse
	 * @return IntegerFormat The parsed integer format
	 */
	static IntegerFormat integer_format_from_string(const String &string);

	/**
	 * @brief Parses string to FloatFormat.
	 *
	 * @param string The string to parse
	 * @return FloatFormat The parsed float format
	 */
	static FloatFormat float_format_from_string(const String &string);

	/**
	 * @brief Custom settings dictionary.
	 *
	 * Stores custom settings that aren't covered by the standard style options.
	 */
	Dictionary custom_settings;

protected:
	/**
	 * @brief Binds methods to make them accessible from GDScript.
	 */
	static void _bind_methods();

private:
	/**
	 * @brief Style option values.
	 */
	ContainerForm container_form = FORM_ANY;
	FlowStyle flow_style = FLOW_ANY;
	StringStyle string_style = STRING_ANY;
	IntegerFormat integer_format = INT_ANY;
	FloatFormat float_format = FLOAT_ANY;

	/**
	 * @brief Flags indicating which options are explicitly set.
	 */
	bool has_container_form = false;
	bool has_flow_style = false;
	bool has_string_style = false;
	bool has_integer_format = false;
	bool has_float_format = false;
	bool has_binary_encoding = false;
	bool has_custom_settings = false;

	/**
	 * @brief Child styles for hierarchical formatting.
	 *
	 * Maps keys to child style objects.
	 */
	std::unordered_map<String, Ref<YAMLStyle>, StringHasher, StringEqual> child_styles;
};

} // namespace godot

/**
 * @brief Variant enum type casting for YAMLStyle enums.
 */
VARIANT_ENUM_CAST(YAMLStyle::ContainerForm);
VARIANT_ENUM_CAST(YAMLStyle::FlowStyle);
VARIANT_ENUM_CAST(YAMLStyle::StringStyle);
VARIANT_ENUM_CAST(YAMLStyle::IntegerFormat);
VARIANT_ENUM_CAST(YAMLStyle::FloatFormat);

#endif // YAML_STYLE_H

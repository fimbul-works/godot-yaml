/**
 * @file packed_byte_array_yaml.h
 * @brief Defines the PackedByteArrayVariantConverter for YAML serialization of Godot PackedByteArray type.
 *
 * This file contains the PackedByteArrayVariantConverter class which handles the conversion
 * between Godot PackedByteArray objects and their YAML representation, with support for
 * different encoding formats such as base64 and hexadecimal.
 */

#ifndef PACKED_BYTE_ARRAY_YAML_H
#define PACKED_BYTE_ARRAY_YAML_H

#include "../variant_converter.h"
#include <string>

namespace godot {

/**
 * @class PackedByteArrayVariantConverter
 * @brief Converter for Godot PackedByteArray type to/from YAML format.
 *
 * The PackedByteArrayVariantConverter class provides functionality to:
 * - Encode PackedByteArray objects into YAML scalar nodes with various encoding formats
 * - Decode YAML scalar nodes into PackedByteArray objects
 * - Apply appropriate formatting based on style settings
 *
 * PackedByteArray objects can be represented in YAML as:
 * - Base64 encoded strings with "base64:" prefix (default)
 * - Hexadecimal encoded strings with "hex:" prefix
 *
 * The converter also handles formatting with appropriate line breaks for long binary data.
 *
 * @extends VariantConverter
 */
class PackedByteArrayVariantConverter : public VariantConverter {
public:
	/**
	 * @brief Defines the YAML tag information.
	 *
	 * - TAG: "PackedByteArray" (used in YAML documents)
	 * - FULL_TAG: "!PackedByteArray" (complete tag with prefix)
	 * - get_tag(): Returns the tag name
	 * - get_full_tag(): Returns the full tag
	 * - get_type(): Returns Variant::PACKED_BYTE_ARRAY
	 */
	DEFINE_YAML_TAG("PackedByteArray", Variant::PACKED_BYTE_ARRAY)

	/**
	 * @brief Encodes a PackedByteArray Variant to a YAML node.
	 *
	 * Converts a Godot PackedByteArray to a YAML scalar node,
	 * as base64 encoded literal string.
	 *
	 * @param node The target YAML node
	 * @param v The PackedByteArray Variant to encode
	 * @param style The style settings to apply
	 */
	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;

	/**
	 * @brief Decodes a YAML node to a PackedByteArray Variant.
	 *
	 * Converts a YAML scalar node to a Godot PackedByteArray,
	 * automatically detecting the encoding format based on prefix ("base64:" or "hex:").
	 * Also supports legacy format without prefix for backward compatibility.
	 *
	 * @param node The source YAML node
	 * @param context The parser context for style detection
	 * @return Variant A PackedByteArray Variant
	 * @throws YAMLException If the node is invalid or contains incorrectly formatted data
	 */
	Variant decode(const ryml::ConstNodeRef &node, ParserContext *context) const override;

private:
	/**
	 * @brief Constants for formatting.
	 */
	static constexpr size_t BASE64_LINE_LENGTH = 76; ///< Standard base64 line length

	/**
	 * @brief Helper struct for string cleanup and format detection.
	 */
	struct CleanupResult {
		String cleaned; ///< Cleaned string without whitespace and prefix
		size_t original_length; ///< Original length of the string before cleaning
	};

	/**
	 * @brief Encoding helper methods.
	 */

	/**
	 * @brief String processing helper methods.
	 */

	/**
	 * @brief Cleans up and detects the encoding format of binary data.
	 *
	 * Detects format based on prefix and cleans up the string by removing whitespace.
	 *
	 * @param input The input string to clean and analyze
	 * @param node The source YAML node (for error context)
	 * @param context The parser context for style detection
	 * @return String The cleaned string and detected format
	 * @throws YAMLException If the data format is invalid
	 */
	String cleanup(const ryml::csubstr &input, const ryml::ConstNodeRef &node, ParserContext *context) const;

	/**
	 * @brief Formats a string with appropriate line breaks.
	 *
	 * Preserves format prefix at the beginning of each line when breaking long lines.
	 *
	 * @param str The string to format
	 * @param line_length The maximum line length
	 * @return ryml::csubstr The formatted string
	 */
	ryml::csubstr format_output(const String &str, size_t line_length) const;

	/**
	 * @brief Checks if a character is a valid base64 character.
	 *
	 * @param c The character to check
	 * @return bool True if the character is a valid base64 character
	 */
	inline const bool is_base64_char(char c) const {
		return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c == '+') || (c == '/') || (c == '=');
	}
};

} // namespace godot

#endif // PACKED_BYTE_ARRAY_YAML_H

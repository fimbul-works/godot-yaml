/**
 * @file variant_converter.hpp
 * @brief Defines the base interface for YAML-to-Variant type converters.
 *
 * This file contains the YAMLVariantConverter abstract base class which provides
 * the interface for converting between YAML nodes and Godot Variant types.
 * Specific type converters inherit from this class to implement conversion
 * logic for each Godot type.
 */
#pragma once

#include "../exception.hpp"
#include "../parser/parser_context.hpp"
#include "../style/style_view.hpp"
#include "../util/string_pool.hpp"

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <ryml.hpp>

/**
 * @brief Macro to define YAML tag information for a YAML Variant Converter class.
 *
 * This macro defines standard methods and constants for YAML tag handling:
 * - TAG: The short tag name (without prefix)
 * - FULL_TAG: The complete tag with prefix
 * - get_tag(): Method to get the short tag name
 * - get_full_tag(): Method to get the full tag
 * - get_type(): Method to get the corresponding Godot Variant type
 *
 * @param TAG_VALUE The tag name string
 * @param VARIANT_TYPE The corresponding Godot Variant::Type
 */
#define DEFINE_YAML_TAG(TAG_VALUE, VARIANT_TYPE)           \
	static constexpr const char *TAG = TAG_VALUE;          \
	static constexpr const char *FULL_TAG = "!" TAG_VALUE; \
	const char *get_tag() const override {                 \
		return TAG;                                        \
	}                                                      \
	const char *get_full_tag() const override {            \
		return FULL_TAG;                                   \
	}                                                      \
	const Variant::Type get_type() const override {        \
		return VARIANT_TYPE;                               \
	}

namespace godot {

/**
 * @class YAMLVariantConverter
 * @brief Abstract base class for YAML-to-Variant type converters.
 *
 * The YAMLVariantConverter class defines the interface for type-specific converters
 * that handle the mapping between YAML nodes and Godot Variant types. Each
 * Godot type has a corresponding converter implementation.
 *
 * Converters handle both encoding (Variant to YAML) and decoding (YAML to Variant)
 * operations, and manage type-specific YAML tag information.
 */
class YAMLVariantConverter {
public:
	/**
	 * @brief Default constructor.
	 */
	explicit YAMLVariantConverter() = default;

	/**
	 * @brief Virtual destructor.
	 */
	virtual ~YAMLVariantConverter() = default;

	/**
	 * @brief Tag identification interface.
	 */

	/**
	 * @brief Gets the tag name without prefix.
	 *
	 * @return const char* The tag name
	 */
	virtual const char *get_tag() const = 0;

	/**
	 * @brief Gets the full tag name with prefix.
	 *
	 * @return const char* The full tag name
	 */
	virtual const char *get_full_tag() const = 0;

	/**
	 * @brief Gets the corresponding Godot Variant type.
	 *
	 * @return const Variant::Type The Variant type
	 */
	virtual const Variant::Type get_type() const = 0;

	/**
	 * @brief Encodes a Variant to a YAML node.
	 *
	 * @param node The target YAML node
	 * @param v The Variant to encode
	 * @param style Style settings for formatting
	 */
	virtual void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const = 0;

	/**
	 * @brief Decodes a YAML node to a Variant.
	 *
	 * @param node The source YAML node
	 * @param context Parsing context information
	 * @return Variant The decoded Variant
	 */
	virtual Variant decode(const ryml::ConstNodeRef &node, YAMLParserContext *context) const = 0;

	/**
	 * @brief Sets the parser reference.
	 *
	 * @param parser Pointer to the ryml parser
	 */
	void set_parser(const ryml::Parser *parser) { m_parser = parser; }

	/**
	 * @brief Gets the parser reference.
	 *
	 * @return const ryml::Parser* Pointer to the ryml parser
	 */
	const ryml::Parser *get_parser() const { return m_parser; }

protected:
	/**
	 * @brief String pool for efficient string handling.
	 */
	mutable YAMLStringPool string_pool;

	/**
	 * @brief Pointer to the ryml parser instance.
	 */
	const ryml::Parser *m_parser = nullptr;

	/**
	 * @brief Stores a string in the string pool.
	 *
	 * @param str The string to store
	 * @return ryml::csubstr View of the stored string
	 */
	ryml::csubstr store_string(const String &str) const {
		return string_pool.store(str);
	}

	/**
	 * @brief Helper methods for exception creation.
	 */

	/**
	 * @brief Creates an exception with location information.
	 *
	 * @param message The error message
	 * @param node The YAML node for location context
	 * @return YAMLException The created exception
	 */
	inline YAMLException create_exception(const String &message, const ryml::ConstNodeRef &node) const {
		if (m_parser && !node.invalid()) {
			try {
				ryml::Location loc = m_parser->location(node);
				return YAMLException(message, loc);
			} catch (...) {
				// If location can't be determined, fall back to basic message
			}
		}

		return YAMLException(message);
	}

	/**
	 * @brief Creates an invalid format exception.
	 *
	 * @param node The YAML node for location context
	 * @return YAMLException The created exception
	 */
	inline YAMLException create_invalid_format_exception(const ryml::ConstNodeRef &node) const {
		return create_exception(vformat("Invalid %s format", get_tag()), node);
	}

	/**
	 * @brief Creates an invalid sequence length exception.
	 *
	 * @param expected_length The expected length
	 * @param node The YAML node for location context
	 * @return YAMLException The created exception
	 */
	inline YAMLException create_invalid_sequence_length_exception(int expected_length, const ryml::ConstNodeRef &node) const {
		return create_exception(vformat("%s array must have %d elements", get_tag(), expected_length), node);
	}

	/**
	 * @brief Creates a decode error exception.
	 *
	 * @param details Error details
	 * @param node The YAML node for location context
	 * @return YAMLException The created exception
	 */
	inline YAMLException create_decode_error_exception(const char *details, const ryml::ConstNodeRef &node) const {
		return create_exception(vformat("Failed to decode %s: %s", get_tag(), details), node);
	}

	/**
	 * @brief Checks if required fields are present in a YAML node.
	 *
	 * @param node The YAML node to check
	 * @param required_fields List of required field names
	 * @throws YAMLException If any required field is missing
	 */
	inline void check_required_fields(const ryml::ConstNodeRef &node, const std::vector<const char *> &required_fields) const {
		std::vector<String> missing_fields;

		for (const char *field : required_fields) {
			if (!node.has_child(field)) {
				missing_fields.push_back(String(field));
			}
		}

		if (!missing_fields.empty()) {
			String missing_list;
			for (size_t i = 0; i < missing_fields.size(); i++) {
				missing_list += missing_fields[i];
				if (i < missing_fields.size() - 1) {
					missing_list += "', '";
				}
			}

			throw create_exception(String(get_tag()) + " missing required field" + (missing_fields.size() > 1 ? "s: " : ": ") + ": '" + missing_list + "'", node);
		}
	}

	/**
	 * @brief Checks if a character is a hexadecimal digit.
	 *
	 * @param c The character to check
	 * @return bool True if the character is a hex digit
	 */
	inline const bool is_hex_char(char c) const {
		return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
	}

	/**
	 * @brief Checks if a character is whitespace.
	 *
	 * @param c The character to check
	 * @return bool True if the character is whitespace
	 */
	inline const bool is_whitespace_char(char c) const {
		return c == ' ' || c == '\t' || c == '\n' || c == '\r';
	}
};

} // namespace godot

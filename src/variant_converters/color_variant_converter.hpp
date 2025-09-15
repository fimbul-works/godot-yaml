/**
 * @file color_variant_converter.hpp
 * @brief Defines the YAMLColorVariantConverter for YAML serialization of Godot Color type.
 *
 * This file contains the YAMLColorVariantConverter class which handles the conversion
 * between Godot Color objects and their YAML representation.
 */
#pragma once

#include "variant_converter.hpp"

namespace godot {

/**
 * @class YAMLColorVariantConverter
 * @brief Converter for Godot Color type to/from YAML format.
 *
 * The YAMLColorVariantConverter class provides functionality to:
 * - Encode Color objects into YAML nodes (as maps, sequences, or HTML color strings)
 * - Decode YAML nodes into Color objects
 * - Apply appropriate formatting based on style settings
 *
 * Color objects can be represented in YAML in multiple formats:
 * - A map with "r", "g", "b", and optionally "a" fields (if alpha < 1.0)
 * - A sequence of RGB or RGBA values
 * - A string in HTML color format (e.g., "#ff0000" or "#ff0000ff" with alpha)
 * - A string with a named color (e.g., "red", "blue")
 *
 * @extends YAMLVariantConverter
 */
class YAMLColorVariantConverter : public YAMLVariantConverter {
public:
	/**
	 * @brief Defines the YAML tag information.
	 *
	 * - TAG: "Color" (used in YAML documents)
	 * - FULL_TAG: "!Color" (complete tag with prefix)
	 * - get_tag(): Returns the tag name
	 * - get_full_tag(): Returns the full tag
	 * - get_type(): Returns Variant::COLOR
	 */
	DEFINE_YAML_TAG("Color", Variant::COLOR)

	/**
	 * @brief Encodes a Color Variant to a YAML node.
	 *
	 * Converts a Godot Color to a YAML node, with format determined by style settings.
	 * Can produce a map, sequence, or string representation depending on configuration.
	 *
	 * @param node The target YAML node
	 * @param v The Color Variant to encode
	 * @param style The style settings to apply
	 */
	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;

	/**
	 * @brief Decodes a YAML node to a Color Variant.
	 *
	 * Converts a YAML node to a Godot Color, handling map, sequence, and string formats.
	 *
	 * @param node The source YAML node
	 * @param context The parser context for style detection
	 * @return Variant A Color Variant
	 * @throws YAMLException If the node format is invalid or incompatible
	 */
	Variant decode(const ryml::ConstNodeRef &node, YAMLParserContext *context) const override;

private:
	/**
	 * @brief Decode helper methods.
	 */

	/**
	 * @brief Decodes a Color from a map-formatted YAML node.
	 *
	 * @param node The source YAML map node
	 * @param context The parser context for style detection
	 * @return Variant The decoded Color object
	 * @throws YAMLException If required fields are missing or invalid
	 */
	Variant decode_map(const ryml::ConstNodeRef &node, YAMLParserContext *context) const;

	/**
	 * @brief Decodes a Color from a sequence-formatted YAML node.
	 *
	 * @param node The source YAML sequence node
	 * @param context The parser context for style detection
	 * @return Variant The decoded Color object
	 * @throws YAMLException If the sequence length is incorrect or elements are invalid
	 */
	Variant decode_sequence(const ryml::ConstNodeRef &node, YAMLParserContext *context) const;

	/**
	 * @brief Decodes a Color from a string-formatted YAML node.
	 *
	 * Handles HTML color codes and named colors.
	 *
	 * @param node The source YAML string node
	 * @return Variant The decoded Color object
	 * @throws YAMLException If the string format is invalid
	 */
	Variant decode_string(const ryml::ConstNodeRef &node) const;

	/**
	 * @brief Validates that no color component is negative.
	 *
	 * @param r Red component
	 * @param g Green component
	 * @param b Blue component
	 * @param a Alpha component
	 * @param node The source YAML node (for error context)
	 * @throws YAMLException If any component is negative
	 */
	void check_negative(real_t r, real_t g, real_t b, real_t a, const ryml::ConstNodeRef &node) const;
};

} // namespace godot

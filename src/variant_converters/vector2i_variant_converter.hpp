/**
 * @file vector2i_variant_converter.hpp
 * @brief Defines the YAMLVector2iVariantConverter for YAML serialization of Godot Vector2i type.
 *
 * This file contains the YAMLVector2iVariantConverter class which handles the conversion
 * between Godot Vector2i objects and their YAML representation.
 */
#pragma once

#include "variant_converter.hpp"

namespace godot {

/**
 * @class YAMLVector2iVariantConverter
 * @brief Converter for Godot Vector2i type to/from YAML format.
 *
 * The YAMLVector2iVariantConverter class provides functionality to:
 * - Encode Vector2i objects into YAML nodes (as maps or sequences)
 * - Decode YAML nodes into Vector2i objects
 * - Apply appropriate formatting based on style settings
 *
 * Vector2i objects are represented in YAML either as:
 * - A map with "x" and "y" fields containing integer values
 * - A sequence of two integer values representing x and y coordinates
 *
 * @extends YAMLVariantConverter
 */
class YAMLVector2iVariantConverter : public YAMLVariantConverter {
public:
	/**
	 * @brief Defines the YAML tag information.
	 *
	 * - TAG: "Vector2i" (used in YAML documents)
	 * - FULL_TAG: "!Vector2i" (complete tag with prefix)
	 * - get_tag(): Returns the tag name
	 * - get_full_tag(): Returns the full tag
	 * - get_type(): Returns Variant::VECTOR2I
	 */
	DEFINE_YAML_TAG("Vector2i", Variant::VECTOR2I)

	/**
	 * @brief Encodes a Vector2i Variant to a YAML node.
	 *
	 * Converts a Godot Vector2i to a YAML node, with format determined by style settings.
	 * Can produce either a map with x/y fields or a sequence of two integer values.
	 *
	 * @param node The target YAML node
	 * @param v The Vector2i Variant to encode
	 * @param style The style settings to apply
	 */
	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;

	/**
	 * @brief Decodes a YAML node to a Vector2i Variant.
	 *
	 * Converts a YAML node to a Godot Vector2i, handling both map and sequence formats.
	 *
	 * @param node The source YAML node
	 * @param context The parser context for style detection
	 * @return Variant A Vector2i Variant
	 * @throws YAMLException If the node format is invalid or incompatible
	 */
	Variant decode(const ryml::ConstNodeRef &node, YAMLParserContext *context) const override;

private:
	/**
	 * @brief Decodes a Vector2i from a map-formatted YAML node.
	 *
	 * Processes a YAML map with "x" and "y" fields into a Vector2i.
	 *
	 * @param node The source YAML map node
	 * @param context The parser context for style detection
	 * @return Vector2i The decoded Vector2i object
	 * @throws YAMLException If required fields are missing or invalid
	 */
	Vector2i decode_from_map(const ryml::ConstNodeRef &node, YAMLParserContext *context) const;

	/**
	 * @brief Decodes a Vector2i from a sequence-formatted YAML node.
	 *
	 * Processes a YAML sequence of two integer values into a Vector2i.
	 *
	 * @param node The source YAML sequence node
	 * @param context The parser context for style detection
	 * @return Vector2i The decoded Vector2i object
	 * @throws YAMLException If the sequence length is incorrect or elements are invalid
	 */
	Vector2i decode_from_sequence(const ryml::ConstNodeRef &node, YAMLParserContext *context) const;
};

} // namespace godot

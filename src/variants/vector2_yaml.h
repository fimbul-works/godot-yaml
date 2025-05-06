/**
 * @file vector2_yaml.h
 * @brief Defines the Vector2VariantConverter for YAML serialization of Godot Vector2 type.
 *
 * This file contains the Vector2VariantConverter class which handles the conversion
 * between Godot Vector2 objects and their YAML representation.
 */

#ifndef VECTOR2_YAML_H
#define VECTOR2_YAML_H

#include "../variant_converter.h"

namespace godot {

/**
 * @class Vector2VariantConverter
 * @brief Converter for Godot Vector2 type to/from YAML format.
 *
 * The Vector2VariantConverter class provides functionality to:
 * - Encode Vector2 objects into YAML nodes (as maps or sequences)
 * - Decode YAML nodes into Vector2 objects
 * - Apply appropriate formatting based on style settings
 *
 * Vector2 objects are represented in YAML either as:
 * - A map with "x" and "y" fields
 * - A sequence of two float values representing x and y coordinates
 *
 * @extends VariantConverter
 */
class Vector2VariantConverter : public VariantConverter {
public:
	/**
	 * @brief Defines the YAML tag information.
	 *
	 * - TAG: "Vector2" (used in YAML documents)
	 * - FULL_TAG: "!Vector2" (complete tag with prefix)
	 * - get_tag(): Returns the tag name
	 * - get_full_tag(): Returns the full tag
	 * - get_type(): Returns Variant::VECTOR2
	 */
	DEFINE_YAML_TAG("Vector2", Variant::VECTOR2)

	/**
	 * @brief Encodes a Vector2 Variant to a YAML node.
	 *
	 * Converts a Godot Vector2 to a YAML node, with format determined by style settings.
	 * Can produce either a map with x/y fields or a sequence of two float values.
	 *
	 * @param node The target YAML node
	 * @param v The Vector2 Variant to encode
	 * @param style The style settings to apply
	 */
	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;

	/**
	 * @brief Decodes a YAML node to a Vector2 Variant.
	 *
	 * Converts a YAML node to a Godot Vector2, handling both map and sequence formats.
	 *
	 * @param node The source YAML node
	 * @param context The parser context for style detection
	 * @return Variant A Vector2 Variant
	 * @throws YAMLException If the node format is invalid or incompatible
	 */
	Variant decode(const ryml::ConstNodeRef &node, ParserContext *context) const override;

private:
	/**
	 * @brief Decodes a Vector2 from a map-formatted YAML node.
	 *
	 * Processes a YAML map with "x" and "y" fields into a Vector2.
	 *
	 * @param node The source YAML map node
	 * @param context The parser context for style detection
	 * @return Vector2 The decoded Vector2 object
	 * @throws YAMLException If required fields are missing or invalid
	 */
	Vector2 decode_from_map(const ryml::ConstNodeRef &node, ParserContext *context) const;

	/**
	 * @brief Decodes a Vector2 from a sequence-formatted YAML node.
	 *
	 * Processes a YAML sequence of two float values into a Vector2.
	 *
	 * @param node The source YAML sequence node
	 * @param context The parser context for style detection
	 * @return Vector2 The decoded Vector2 object
	 * @throws YAMLException If the sequence length is incorrect or elements are invalid
	 */
	Vector2 decode_from_sequence(const ryml::ConstNodeRef &node, ParserContext *context) const;
};

} // namespace godot

#endif // VECTOR2_YAML_H

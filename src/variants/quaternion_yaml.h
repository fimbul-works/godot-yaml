/**
 * @file quaternion_yaml.h
 * @brief Defines the QuaternionVariantConverter for YAML serialization of Godot Quaternion type.
 *
 * This file contains the QuaternionVariantConverter class which handles the conversion
 * between Godot Quaternion objects and their YAML representation.
 */

#ifndef QUATERNION_YAML_H
#define QUATERNION_YAML_H

#include "../variant_converter.h"

namespace godot {

/**
 * @class QuaternionVariantConverter
 * @brief Converter for Godot Quaternion type to/from YAML format.
 *
 * The QuaternionVariantConverter class provides functionality to:
 * - Encode Quaternion objects into YAML nodes (as maps or sequences)
 * - Decode YAML nodes into Quaternion objects
 * - Apply appropriate formatting based on style settings
 *
 * Quaternion objects are represented in YAML either as:
 * - A map with "x", "y", "z", and "w" fields
 * - A sequence of four float values representing x, y, z, and w components
 *
 * @extends VariantConverter
 */
class QuaternionVariantConverter : public VariantConverter {
public:
	/**
	 * @brief Defines the YAML tag information.
	 *
	 * - TAG: "Quaternion" (used in YAML documents)
	 * - FULL_TAG: "!Quaternion" (complete tag with prefix)
	 * - get_tag(): Returns the tag name
	 * - get_full_tag(): Returns the full tag
	 * - get_type(): Returns Variant::QUATERNION
	 */
	DEFINE_YAML_TAG("Quaternion", Variant::QUATERNION)

	/**
	 * @brief Encodes a Quaternion Variant to a YAML node.
	 *
	 * Converts a Godot Quaternion to a YAML node, with format determined by style settings.
	 * Can produce either a map with x/y/z/w fields or a sequence of four float values.
	 *
	 * @param node The target YAML node
	 * @param v The Quaternion Variant to encode
	 * @param style The style settings to apply
	 */
	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;

	/**
	 * @brief Decodes a YAML node to a Quaternion Variant.
	 *
	 * Converts a YAML node to a Godot Quaternion, handling both map and sequence formats.
	 *
	 * @param node The source YAML node
	 * @param context The parser context for style detection
	 * @return Variant A Quaternion Variant
	 * @throws YAMLException If the node format is invalid or incompatible
	 */
	Variant decode(const ryml::ConstNodeRef &node, ParserContext *context) const override;

private:
	/**
	 * @brief Decodes a Quaternion from a map-formatted YAML node.
	 *
	 * Processes a YAML map with "x", "y", "z", and "w" fields into a Quaternion.
	 *
	 * @param node The source YAML map node
	 * @param context The parser context for style detection
	 * @return Quaternion The decoded Quaternion object
	 * @throws YAMLException If required fields are missing or invalid
	 */
	Quaternion decode_from_map(const ryml::ConstNodeRef &node, ParserContext *context) const;

	/**
	 * @brief Decodes a Quaternion from a sequence-formatted YAML node.
	 *
	 * Processes a YAML sequence of four float values into a Quaternion.
	 *
	 * @param node The source YAML sequence node
	 * @param context The parser context for style detection
	 * @return Quaternion The decoded Quaternion object
	 * @throws YAMLException If the sequence length is incorrect or elements are invalid
	 */
	Quaternion decode_from_sequence(const ryml::ConstNodeRef &node, ParserContext *context) const;
};

} // namespace godot

#endif // QUATERNION_YAML_H

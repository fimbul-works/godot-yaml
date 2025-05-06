/**
 * @file vector3_yaml.h
 * @brief Defines the Vector3VariantConverter for YAML serialization of Godot Vector3 type.
 *
 * This file contains the Vector3VariantConverter class which handles the conversion
 * between Godot Vector3 objects and their YAML representation.
 */

#ifndef VECTOR3_YAML_H
#define VECTOR3_YAML_H

#include "../variant_converter.h"

namespace godot {

/**
 * @class Vector3VariantConverter
 * @brief Converter for Godot Vector3 type to/from YAML format.
 *
 * The Vector3VariantConverter class provides functionality to:
 * - Encode Vector3 objects into YAML nodes (as maps or sequences)
 * - Decode YAML nodes into Vector3 objects
 * - Apply appropriate formatting based on style settings
 *
 * Vector3 objects are represented in YAML either as:
 * - A map with "x", "y", and "z" fields
 * - A sequence of three float values representing x, y, and z coordinates
 *
 * @extends VariantConverter
 */
class Vector3VariantConverter : public VariantConverter {
public:
	/**
	 * @brief Defines the YAML tag information.
	 *
	 * - TAG: "Vector3" (used in YAML documents)
	 * - FULL_TAG: "!Vector3" (complete tag with prefix)
	 * - get_tag(): Returns the tag name
	 * - get_full_tag(): Returns the full tag
	 * - get_type(): Returns Variant::VECTOR3
	 */
	DEFINE_YAML_TAG("Vector3", Variant::VECTOR3)

	/**
	 * @brief Encodes a Vector3 Variant to a YAML node.
	 *
	 * Converts a Godot Vector3 to a YAML node, with format determined by style settings.
	 * Can produce either a map with x/y/z fields or a sequence of three float values.
	 *
	 * @param node The target YAML node
	 * @param v The Vector3 Variant to encode
	 * @param style The style settings to apply
	 */
	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;

	/**
	 * @brief Decodes a YAML node to a Vector3 Variant.
	 *
	 * Converts a YAML node to a Godot Vector3, handling both map and sequence formats.
	 *
	 * @param node The source YAML node
	 * @param context The parser context for style detection
	 * @return Variant A Vector3 Variant
	 * @throws YAMLException If the node format is invalid or incompatible
	 */
	Variant decode(const ryml::ConstNodeRef &node, ParserContext *context) const override;

private:
	/**
	 * @brief Decodes a Vector3 from a map-formatted YAML node.
	 *
	 * Processes a YAML map with "x", "y", and "z" fields into a Vector3.
	 *
	 * @param node The source YAML map node
	 * @param context The parser context for style detection
	 * @return Vector3 The decoded Vector3 object
	 * @throws YAMLException If required fields are missing or invalid
	 */
	Vector3 decode_from_map(const ryml::ConstNodeRef &node, ParserContext *context) const;

	/**
	 * @brief Decodes a Vector3 from a sequence-formatted YAML node.
	 *
	 * Processes a YAML sequence of three float values into a Vector3.
	 *
	 * @param node The source YAML sequence node
	 * @param context The parser context for style detection
	 * @return Vector3 The decoded Vector3 object
	 * @throws YAMLException If the sequence length is incorrect or elements are invalid
	 */
	Vector3 decode_from_sequence(const ryml::ConstNodeRef &node, ParserContext *context) const;
};

} // namespace godot

#endif // VECTOR3_YAML_H

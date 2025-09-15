/**
 * @file vector3i_variant_converter.hpp
 * @brief Defines the YAMLVector3iVariantConverter for YAML serialization of Godot Vector3i type.
 *
 * This file contains the YAMLVector3iVariantConverter class which handles the conversion
 * between Godot Vector3i objects and their YAML representation.
 */
#pragma once

#include "variant_converter.hpp"

namespace godot {

/**
 * @class YAMLVector3iVariantConverter
 * @brief Converter for Godot Vector3i type to/from YAML format.
 *
 * The YAMLVector3iVariantConverter class provides functionality to:
 * - Encode Vector3i objects into YAML nodes (as maps or sequences)
 * - Decode YAML nodes into Vector3i objects
 * - Apply appropriate formatting based on style settings
 *
 * Vector3i objects are represented in YAML either as:
 * - A map with "x", "y", and "z" fields containing integer values
 * - A sequence of three integer values representing x, y, and z coordinates
 *
 * @extends YAMLVariantConverter
 */
class YAMLVector3iVariantConverter : public YAMLVariantConverter {
public:
	/**
	 * @brief Defines the YAML tag information.
	 *
	 * - TAG: "Vector3i" (used in YAML documents)
	 * - FULL_TAG: "!Vector3i" (complete tag with prefix)
	 * - get_tag(): Returns the tag name
	 * - get_full_tag(): Returns the full tag
	 * - get_type(): Returns Variant::VECTOR3I
	 */
	DEFINE_YAML_TAG("Vector3i", Variant::VECTOR3I)

	/**
	 * @brief Encodes a Vector3i Variant to a YAML node.
	 *
	 * Converts a Godot Vector3i to a YAML node, with format determined by style settings.
	 * Can produce either a map with x/y/z fields or a sequence of three integer values.
	 *
	 * @param node The target YAML node
	 * @param v The Vector3i Variant to encode
	 * @param style The style settings to apply
	 */
	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;

	/**
	 * @brief Decodes a YAML node to a Vector3i Variant.
	 *
	 * Converts a YAML node to a Godot Vector3i, handling both map and sequence formats.
	 *
	 * @param node The source YAML node
	 * @param context The parser context for style detection
	 * @return Variant A Vector3i Variant
	 * @throws YAMLException If the node format is invalid or incompatible
	 */
	Variant decode(const ryml::ConstNodeRef &node, YAMLParserContext *context) const override;

private:
	/**
	 * @brief Decodes a Vector3i from a map-formatted YAML node.
	 *
	 * Processes a YAML map with "x", "y", and "z" fields into a Vector3i.
	 *
	 * @param node The source YAML map node
	 * @param context The parser context for style detection
	 * @return Vector3i The decoded Vector3i object
	 * @throws YAMLException If required fields are missing or invalid
	 */
	Vector3i decode_from_map(const ryml::ConstNodeRef &node, YAMLParserContext *context) const;

	/**
	 * @brief Decodes a Vector3i from a sequence-formatted YAML node.
	 *
	 * Processes a YAML sequence of three integer values into a Vector3i.
	 *
	 * @param node The source YAML sequence node
	 * @param context The parser context for style detection
	 * @return Vector3i The decoded Vector3i object
	 * @throws YAMLException If the sequence length is incorrect or elements are invalid
	 */
	Vector3i decode_from_sequence(const ryml::ConstNodeRef &node, YAMLParserContext *context) const;
};

} // namespace godot

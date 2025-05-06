/**
 * @file vector4i_yaml.h
 * @brief Defines the Vector4iVariantConverter for YAML serialization of Godot Vector4i type.
 *
 * This file contains the Vector4iVariantConverter class which handles the conversion
 * between Godot Vector4i objects and their YAML representation.
 */

#ifndef VECTOR4I_YAML_H
#define VECTOR4I_YAML_H

#include "../variant_converter.h"

namespace godot {

/**
 * @class Vector4iVariantConverter
 * @brief Converter for Godot Vector4i type to/from YAML format.
 *
 * The Vector4iVariantConverter class provides functionality to:
 * - Encode Vector4i objects into YAML nodes (as maps or sequences)
 * - Decode YAML nodes into Vector4i objects
 * - Apply appropriate formatting based on style settings
 *
 * Vector4i objects are represented in YAML either as:
 * - A map with "x", "y", "z", and "w" fields containing integer values
 * - A sequence of four integer values representing x, y, z, and w coordinates
 *
 * @extends VariantConverter
 */
class Vector4iVariantConverter : public VariantConverter {
public:
	/**
	 * @brief Defines the YAML tag information.
	 *
	 * - TAG: "Vector4i" (used in YAML documents)
	 * - FULL_TAG: "!Vector4i" (complete tag with prefix)
	 * - get_tag(): Returns the tag name
	 * - get_full_tag(): Returns the full tag
	 * - get_type(): Returns Variant::VECTOR4I
	 */
	DEFINE_YAML_TAG("Vector4i", Variant::VECTOR4I)

	/**
	 * @brief Encodes a Vector4i Variant to a YAML node.
	 *
	 * Converts a Godot Vector4i to a YAML node, with format determined by style settings.
	 * Can produce either a map with x/y/z/w fields or a sequence of four integer values.
	 *
	 * @param node The target YAML node
	 * @param v The Vector4i Variant to encode
	 * @param style The style settings to apply
	 */
	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;

	/**
	 * @brief Decodes a YAML node to a Vector4i Variant.
	 *
	 * Converts a YAML node to a Godot Vector4i, handling both map and sequence formats.
	 *
	 * @param node The source YAML node
	 * @param context The parser context for style detection
	 * @return Variant A Vector4i Variant
	 * @throws YAMLException If the node format is invalid or incompatible
	 */
	Variant decode(const ryml::ConstNodeRef &node, ParserContext *context) const override;

private:
	/**
	 * @brief Decodes a Vector4i from a map-formatted YAML node.
	 *
	 * Processes a YAML map with "x", "y", "z", and "w" fields into a Vector4i.
	 *
	 * @param node The source YAML map node
	 * @param context The parser context for style detection
	 * @return Vector4i The decoded Vector4i object
	 * @throws YAMLException If required fields are missing or invalid
	 */
	Vector4i decode_from_map(const ryml::ConstNodeRef &node, ParserContext *context) const;

	/**
	 * @brief Decodes a Vector4i from a sequence-formatted YAML node.
	 *
	 * Processes a YAML sequence of four integer values into a Vector4i.
	 *
	 * @param node The source YAML sequence node
	 * @param context The parser context for style detection
	 * @return Vector4i The decoded Vector4i object
	 * @throws YAMLException If the sequence length is incorrect or elements are invalid
	 */
	Vector4i decode_from_sequence(const ryml::ConstNodeRef &node, ParserContext *context) const;
};

} // namespace godot

#endif // VECTOR4I_YAML_H

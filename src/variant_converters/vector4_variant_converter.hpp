/**
 * @file vector4_variant_converter.hpp
 * @brief Defines the YAMLVector4VariantConverter for YAML serialization of Godot Vector4 type.
 *
 * This file contains the YAMLVector4VariantConverter class which handles the conversion
 * between Godot Vector4 objects and their YAML representation.
 */
#pragma once

#include "variant_converter.hpp"

namespace godot {

/**
 * @class YAMLVector4VariantConverter
 * @brief Converter for Godot Vector4 type to/from YAML format.
 *
 * The YAMLVector4VariantConverter class provides functionality to:
 * - Encode Vector4 objects into YAML nodes (as maps or sequences)
 * - Decode YAML nodes into Vector4 objects
 * - Apply appropriate formatting based on style settings
 *
 * Vector4 objects are represented in YAML either as:
 * - A map with "x", "y", "z", and "w" fields
 * - A sequence of four float values representing x, y, z, and w coordinates
 *
 * @extends YAMLVariantConverter
 */
class YAMLVector4VariantConverter : public YAMLVariantConverter {
public:
	/**
	 * @brief Defines the YAML tag information.
	 *
	 * - TAG: "Vector4" (used in YAML documents)
	 * - FULL_TAG: "!Vector4" (complete tag with prefix)
	 * - get_tag(): Returns the tag name
	 * - get_full_tag(): Returns the full tag
	 * - get_type(): Returns Variant::VECTOR4
	 */
	DEFINE_YAML_TAG("Vector4", Variant::VECTOR4)

	/**
	 * @brief Encodes a Vector4 Variant to a YAML node.
	 *
	 * Converts a Godot Vector4 to a YAML node, with format determined by style settings.
	 * Can produce either a map with x/y/z/w fields or a sequence of four float values.
	 *
	 * @param node The target YAML node
	 * @param v The Vector4 Variant to encode
	 * @param style The style settings to apply
	 */
	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;

	/**
	 * @brief Decodes a YAML node to a Vector4 Variant.
	 *
	 * Converts a YAML node to a Godot Vector4, handling both map and sequence formats.
	 *
	 * @param node The source YAML node
	 * @param context The parser context for style detection
	 * @return Variant A Vector4 Variant
	 * @throws YAMLException If the node format is invalid or incompatible
	 */
	Variant decode(const ryml::ConstNodeRef &node, YAMLParserContext *context) const override;

private:
	/**
	 * @brief Decodes a Vector4 from a map-formatted YAML node.
	 *
	 * Processes a YAML map with "x", "y", "z", and "w" fields into a Vector4.
	 *
	 * @param node The source YAML map node
	 * @param context The parser context for style detection
	 * @return Vector4 The decoded Vector4 object
	 * @throws YAMLException If required fields are missing or invalid
	 */
	Vector4 decode_from_map(const ryml::ConstNodeRef &node, YAMLParserContext *context) const;

	/**
	 * @brief Decodes a Vector4 from a sequence-formatted YAML node.
	 *
	 * Processes a YAML sequence of four float values into a Vector4.
	 *
	 * @param node The source YAML sequence node
	 * @param context The parser context for style detection
	 * @return Vector4 The decoded Vector4 object
	 * @throws YAMLException If the sequence length is incorrect or elements are invalid
	 */
	Vector4 decode_from_sequence(const ryml::ConstNodeRef &node, YAMLParserContext *context) const;
};

} // namespace godot

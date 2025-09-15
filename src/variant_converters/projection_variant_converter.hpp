/**
 * @file projection_variant_converter.hpp
 * @brief Defines the YAMLProjectionVariantConverter for YAML serialization of Godot Projection type.
 *
 * This file contains the YAMLProjectionVariantConverter class which handles the conversion
 * between Godot Projection objects and their YAML representation.
 */
#pragma once

#include "variant_converter.hpp"
#include "variant_converter_factory.hpp"
#include "vector4_variant_converter.hpp"

namespace godot {

/**
 * @class YAMLProjectionVariantConverter
 * @brief Converter for Godot Projection type to/from YAML format.
 *
 * The YAMLProjectionVariantConverter class provides functionality to:
 * - Encode Projection objects into YAML nodes (as maps or sequences)
 * - Decode YAML nodes into Projection objects
 * - Apply appropriate formatting based on style settings
 *
 * Projection objects are represented in YAML either as:
 * - A map with "x", "y", "z", and "w" fields, each containing Vector4 values for matrix columns
 * - A sequence of four Vector4 values representing the matrix columns
 *
 * @extends YAMLVariantConverter
 */
class YAMLProjectionVariantConverter : public YAMLVariantConverter {
public:
	/**
	 * @brief Constructor that initializes the converter.
	 *
	 * Initializes the Vector4 converter required for Projection column encoding.
	 *
	 * @param factory Pointer to the converter factory
	 */
	explicit YAMLProjectionVariantConverter(YAMLVariantConverterFactory *factory);

	/**
	 * @brief Defines the YAML tag information.
	 *
	 * - TAG: "Projection" (used in YAML documents)
	 * - FULL_TAG: "!Projection" (complete tag with prefix)
	 * - get_tag(): Returns the tag name
	 * - get_full_tag(): Returns the full tag
	 * - get_type(): Returns Variant::PROJECTION
	 */
	DEFINE_YAML_TAG("Projection", Variant::PROJECTION)

	/**
	 * @brief Encodes a Projection Variant to a YAML node.
	 *
	 * Converts a Godot Projection to a YAML node, with format determined by style settings.
	 * Can produce either a map with x/y/z/w fields or a sequence of four Vector4 values.
	 *
	 * @param node The target YAML node
	 * @param v The Projection Variant to encode
	 * @param style The style settings to apply
	 */
	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;

	/**
	 * @brief Decodes a YAML node to a Projection Variant.
	 *
	 * Converts a YAML node to a Godot Projection, handling both map and sequence formats.
	 *
	 * @param node The source YAML node
	 * @param context The parser context for style detection
	 * @return Variant A Projection Variant
	 * @throws YAMLException If the node format is invalid or incompatible
	 */
	Variant decode(const ryml::ConstNodeRef &node, YAMLParserContext *context) const override;

private:
	/**
	 * @brief Decodes a Projection from a map-formatted YAML node.
	 *
	 * Processes a YAML map with "x", "y", "z", and "w" fields into a Projection.
	 *
	 * @param node The source YAML map node
	 * @param context The parser context for style detection
	 * @return Variant The decoded Projection object
	 * @throws YAMLException If required fields are missing or invalid
	 */
	Variant decode_from_map(const ryml::ConstNodeRef &node, YAMLParserContext *context) const;

	/**
	 * @brief Decodes a Projection from a sequence-formatted YAML node.
	 *
	 * Processes a YAML sequence of four Vector4 values into a Projection.
	 *
	 * @param node The source YAML sequence node
	 * @param context The parser context for style detection
	 * @return Variant The decoded Projection object
	 * @throws YAMLException If the sequence length is incorrect or elements are invalid
	 */
	Variant decode_from_sequence(const ryml::ConstNodeRef &node, YAMLParserContext *context) const;

	/**
	 * @brief Converter for Vector4 components.
	 *
	 * Used to encode/decode the column vectors of the Projection matrix.
	 */
	std::unique_ptr<YAMLVector4VariantConverter> vec4_converter;
};

} // namespace godot

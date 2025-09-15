/**
 * @file aabb_variant_converter.hpp
 * @brief Defines the YAMLAABBVariantConverter for YAML serialization of Godot AABB type.
 *
 * This file contains the YAMLAABBVariantConverter class which handles the conversion
 * between Godot AABB (Axis-Aligned Bounding Box) objects and their YAML representation.
 */
#pragma once

#include "variant_converter.hpp"
#include "variant_converter_factory.hpp"
#include "vector3_variant_converter.hpp"

namespace godot {

/**
 * @class YAMLAABBVariantConverter
 * @brief Converter for Godot AABB type to/from YAML format.
 *
 * The YAMLAABBVariantConverter class provides functionality to:
 * - Encode AABB objects into YAML nodes (as maps or sequences)
 * - Decode YAML nodes into AABB objects
 * - Apply appropriate formatting based on style settings
 *
 * AABB objects are represented in YAML either as:
 * - A map with "position" and "size" fields, each containing Vector3 values
 * - A sequence of two Vector3 values representing position and size
 *
 * @extends YAMLVariantConverter
 */
class YAMLAABBVariantConverter : public YAMLVariantConverter {
public:
	/**
	 * @brief Constructor that initializes the converter.
	 *
	 * Initializes the Vector3 converter required for AABB component encoding.
	 *
	 * @param factory Pointer to the converter factory
	 */
	explicit YAMLAABBVariantConverter(YAMLVariantConverterFactory *factory);

	/**
	 * @brief Defines the YAML tag information.
	 *
	 * - TAG: "AABB" (used in YAML documents)
	 * - FULL_TAG: "!AABB" (complete tag with prefix)
	 * - get_tag(): Returns the tag name
	 * - get_full_tag(): Returns the full tag
	 * - get_type(): Returns Variant::AABB
	 */
	DEFINE_YAML_TAG("AABB", Variant::AABB)

	/**
	 * @brief Encodes an AABB Variant to a YAML node.
	 *
	 * Converts a Godot AABB to a YAML node, with format determined by style settings.
	 * Can produce either a map with position/size fields or a sequence of position and size values.
	 *
	 * @param node The target YAML node
	 * @param v The AABB Variant to encode
	 * @param style The style settings to apply
	 */
	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;

	/**
	 * @brief Decodes a YAML node to an AABB Variant.
	 *
	 * Converts a YAML node to a Godot AABB, handling both map and sequence formats.
	 *
	 * @param node The source YAML node
	 * @param context The parser context for style detection
	 * @return Variant An AABB Variant
	 * @throws YAMLException If the node format is invalid or incompatible
	 */
	Variant decode(const ryml::ConstNodeRef &node, YAMLParserContext *context) const override;

private:
	/**
	 * @brief Decodes an AABB from a map-formatted YAML node.
	 *
	 * Processes a YAML map with "position" and "size" fields into an AABB.
	 *
	 * @param node The source YAML map node
	 * @param context The parser context for style detection
	 * @return Variant The decoded AABB object
	 * @throws YAMLException If required fields are missing or invalid
	 */
	Variant decode_from_map(const ryml::ConstNodeRef &node, YAMLParserContext *context) const;

	/**
	 * @brief Decodes an AABB from a sequence-formatted YAML node.
	 *
	 * Processes a YAML sequence of position and size values into an AABB.
	 *
	 * @param node The source YAML sequence node
	 * @param context The parser context for style detection
	 * @return Variant The decoded AABB object
	 * @throws YAMLException If the sequence length is incorrect or elements are invalid
	 */
	Variant decode_from_sequence(const ryml::ConstNodeRef &node, YAMLParserContext *context) const;

	/**
	 * @brief Converter for Vector3 components.
	 *
	 * Used to encode/decode the position and size Vector3 components of the AABB.
	 */
	std::unique_ptr<YAMLVector3VariantConverter> vec3_converter;
};

} // namespace godot

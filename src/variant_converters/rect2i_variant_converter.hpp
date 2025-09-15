/**
 * @file rect2i_variant_converter.hpp
 * @brief Defines the YAMLRect2iVariantConverter for YAML serialization of Godot Rect2i type.
 *
 * This file contains the YAMLRect2iVariantConverter class which handles the conversion
 * between Godot Rect2i objects and their YAML representation.
 */
#pragma once

#include "variant_converter.hpp"
#include "variant_converter_factory.hpp"
#include "vector2i_variant_converter.hpp"

namespace godot {

/**
 * @class YAMLRect2iVariantConverter
 * @brief Converter for Godot Rect2i type to/from YAML format.
 *
 * The YAMLRect2iVariantConverter class provides functionality to:
 * - Encode Rect2i objects into YAML nodes (as maps or sequences)
 * - Decode YAML nodes into Rect2i objects
 * - Apply appropriate formatting based on style settings
 *
 * Rect2i objects are represented in YAML either as:
 * - A map with "position" and "size" fields, each containing Vector2i values
 * - A sequence of two Vector2i values representing position and size
 *
 * @extends YAMLVariantConverter
 */
class YAMLRect2iVariantConverter : public YAMLVariantConverter {
public:
	/**
	 * @brief Constructor that initializes the converter.
	 *
	 * Initializes the Vector2i converter required for Rect2i component encoding.
	 *
	 * @param factory Pointer to the converter factory
	 */
	explicit YAMLRect2iVariantConverter(YAMLVariantConverterFactory *factory);

	/**
	 * @brief Defines the YAML tag information.
	 *
	 * - TAG: "Rect2i" (used in YAML documents)
	 * - FULL_TAG: "!Rect2i" (complete tag with prefix)
	 * - get_tag(): Returns the tag name
	 * - get_full_tag(): Returns the full tag
	 * - get_type(): Returns Variant::RECT2I
	 */
	DEFINE_YAML_TAG("Rect2i", Variant::RECT2I)

	/**
	 * @brief Encodes a Rect2i Variant to a YAML node.
	 *
	 * Converts a Godot Rect2i to a YAML node, with format determined by style settings.
	 * Can produce either a map with position/size fields or a sequence of position and size values.
	 *
	 * @param node The target YAML node
	 * @param v The Rect2i Variant to encode
	 * @param style The style settings to apply
	 */
	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;

	/**
	 * @brief Decodes a YAML node to a Rect2i Variant.
	 *
	 * Converts a YAML node to a Godot Rect2i, handling both map and sequence formats.
	 *
	 * @param node The source YAML node
	 * @param context The parser context for style detection
	 * @return Variant A Rect2i Variant
	 * @throws YAMLException If the node format is invalid or incompatible
	 */
	Variant decode(const ryml::ConstNodeRef &node, YAMLParserContext *context) const override;

private:
	/**
	 * @brief Decodes a Rect2i from a map-formatted YAML node.
	 *
	 * Processes a YAML map with "position" and "size" fields into a Rect2i.
	 *
	 * @param node The source YAML map node
	 * @param context The parser context for style detection
	 * @return Rect2i The decoded Rect2i object
	 * @throws YAMLException If required fields are missing or invalid
	 */
	Rect2i decode_from_map(const ryml::ConstNodeRef &node, YAMLParserContext *context) const;

	/**
	 * @brief Decodes a Rect2i from a sequence-formatted YAML node.
	 *
	 * Processes a YAML sequence of two Vector2i values into a Rect2i.
	 *
	 * @param node The source YAML sequence node
	 * @param context The parser context for style detection
	 * @return Rect2i The decoded Rect2i object
	 * @throws YAMLException If the sequence length is incorrect or elements are invalid
	 */
	Rect2i decode_from_sequence(const ryml::ConstNodeRef &node, YAMLParserContext *context) const;

	/**
	 * @brief Converter for Vector2i components.
	 *
	 * Used to encode/decode the position and size Vector2i components of the Rect2i.
	 */
	std::unique_ptr<YAMLVector2iVariantConverter> vec2i_converter;
};

} // namespace godot

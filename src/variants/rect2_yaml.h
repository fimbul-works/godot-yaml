/**
 * @file rect2_yaml.h
 * @brief Defines the Rect2VariantConverter for YAML serialization of Godot Rect2 type.
 *
 * This file contains the Rect2VariantConverter class which handles the conversion
 * between Godot Rect2 objects and their YAML representation.
 */

#ifndef RECT2_YAML_H
#define RECT2_YAML_H

#include "../converter_factory.h"
#include "../variant_converter.h"
#include "vector2_yaml.h"

namespace godot {

/**
 * @class Rect2VariantConverter
 * @brief Converter for Godot Rect2 type to/from YAML format.
 *
 * The Rect2VariantConverter class provides functionality to:
 * - Encode Rect2 objects into YAML nodes (as maps or sequences)
 * - Decode YAML nodes into Rect2 objects
 * - Apply appropriate formatting based on style settings
 *
 * Rect2 objects are represented in YAML either as:
 * - A map with "position" and "size" fields, each containing Vector2 values
 * - A sequence of two Vector2 values representing position and size
 *
 * @extends VariantConverter
 */
class Rect2VariantConverter : public VariantConverter {
public:
	/**
	 * @brief Constructor that initializes the converter.
	 *
	 * Initializes the Vector2 converter required for Rect2 component encoding.
	 *
	 * @param factory Pointer to the converter factory
	 */
	explicit Rect2VariantConverter(ConverterFactory *factory);

	/**
	 * @brief Defines the YAML tag information.
	 *
	 * - TAG: "Rect2" (used in YAML documents)
	 * - FULL_TAG: "!Rect2" (complete tag with prefix)
	 * - get_tag(): Returns the tag name
	 * - get_full_tag(): Returns the full tag
	 * - get_type(): Returns Variant::RECT2
	 */
	DEFINE_YAML_TAG("Rect2", Variant::RECT2)

	/**
	 * @brief Encodes a Rect2 Variant to a YAML node.
	 *
	 * Converts a Godot Rect2 to a YAML node, with format determined by style settings.
	 * Can produce either a map with position/size fields or a sequence of position and size values.
	 *
	 * @param node The target YAML node
	 * @param v The Rect2 Variant to encode
	 * @param style The style settings to apply
	 */
	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;

	/**
	 * @brief Decodes a YAML node to a Rect2 Variant.
	 *
	 * Converts a YAML node to a Godot Rect2, handling both map and sequence formats.
	 *
	 * @param node The source YAML node
	 * @param context The parser context for style detection
	 * @return Variant A Rect2 Variant
	 * @throws YAMLException If the node format is invalid or incompatible
	 */
	Variant decode(const ryml::ConstNodeRef &node, ParserContext *context) const override;

private:
	/**
	 * @brief Decodes a Rect2 from a map-formatted YAML node.
	 *
	 * Processes a YAML map with "position" and "size" fields into a Rect2.
	 *
	 * @param node The source YAML map node
	 * @param context The parser context for style detection
	 * @return Rect2 The decoded Rect2 object
	 * @throws YAMLException If required fields are missing or invalid
	 */
	Rect2 decode_from_map(const ryml::ConstNodeRef &node, ParserContext *context) const;

	/**
	 * @brief Decodes a Rect2 from a sequence-formatted YAML node.
	 *
	 * Processes a YAML sequence of two Vector2 values into a Rect2.
	 *
	 * @param node The source YAML sequence node
	 * @param context The parser context for style detection
	 * @return Rect2 The decoded Rect2 object
	 * @throws YAMLException If the sequence length is incorrect or elements are invalid
	 */
	Rect2 decode_from_sequence(const ryml::ConstNodeRef &node, ParserContext *context) const;

	/**
	 * @brief Converter for Vector2 components.
	 *
	 * Used to encode/decode the position and size Vector2 components of the Rect2.
	 */
	std::unique_ptr<Vector2VariantConverter> vec2_converter;
};

} // namespace godot

#endif // RECT2_YAML_H

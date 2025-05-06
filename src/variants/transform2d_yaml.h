/**
 * @file transform2d_yaml.h
 * @brief Defines the Transform2DVariantConverter for YAML serialization of Godot Transform2D type.
 *
 * This file contains the Transform2DVariantConverter class which handles the conversion
 * between Godot Transform2D objects and their YAML representation.
 */

#ifndef TRANSFORM2D_YAML_H
#define TRANSFORM2D_YAML_H

#include "../converter_factory.h"
#include "../variant_converter.h"
#include "vector2_yaml.h"

namespace godot {

/**
 * @class Transform2DVariantConverter
 * @brief Converter for Godot Transform2D type to/from YAML format.
 *
 * The Transform2DVariantConverter class provides functionality to:
 * - Encode Transform2D objects into YAML nodes (as maps or sequences)
 * - Decode YAML nodes into Transform2D objects
 * - Apply appropriate formatting based on style settings
 *
 * Transform2D objects are represented in YAML either as:
 * - A map with "x", "y", and "origin" fields, each containing Vector2 values
 * - A sequence of three Vector2 values representing x, y, and origin columns
 *
 * @extends VariantConverter
 */
class Transform2DVariantConverter : public VariantConverter {
public:
	/**
	 * @brief Constructor that initializes the converter.
	 *
	 * Initializes the Vector2 converter required for Transform2D column encoding.
	 *
	 * @param factory Pointer to the converter factory
	 */
	explicit Transform2DVariantConverter(ConverterFactory *factory);

	/**
	 * @brief Defines the YAML tag information.
	 *
	 * - TAG: "Transform2D" (used in YAML documents)
	 * - FULL_TAG: "!Transform2D" (complete tag with prefix)
	 * - get_tag(): Returns the tag name
	 * - get_full_tag(): Returns the full tag
	 * - get_type(): Returns Variant::TRANSFORM2D
	 */
	DEFINE_YAML_TAG("Transform2D", Variant::TRANSFORM2D)

	/**
	 * @brief Encodes a Transform2D Variant to a YAML node.
	 *
	 * Converts a Godot Transform2D to a YAML node, with format determined by style settings.
	 * Can produce either a map with named fields or a sequence of Vector2 values.
	 *
	 * @param node The target YAML node
	 * @param v The Transform2D Variant to encode
	 * @param style The style settings to apply
	 */
	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;

	/**
	 * @brief Decodes a YAML node to a Transform2D Variant.
	 *
	 * Converts a YAML node to a Godot Transform2D, handling both map and sequence formats.
	 *
	 * @param node The source YAML node
	 * @param context The parser context for style detection
	 * @return Variant A Transform2D Variant
	 * @throws YAMLException If the node format is invalid or incompatible
	 */
	Variant decode(const ryml::ConstNodeRef &node, ParserContext *context) const override;

private:
	/**
	 * @brief Decodes a Transform2D from a map-formatted YAML node.
	 *
	 * Processes a YAML map with "x", "y", and "origin" fields into a Transform2D.
	 *
	 * @param node The source YAML map node
	 * @param context The parser context for style detection
	 * @return Transform2D The decoded Transform2D object
	 * @throws YAMLException If required fields are missing or invalid
	 */
	Transform2D decode_from_map(const ryml::ConstNodeRef &node, ParserContext *context) const;

	/**
	 * @brief Decodes a Transform2D from a sequence-formatted YAML node.
	 *
	 * Processes a YAML sequence of three Vector2 values into a Transform2D.
	 *
	 * @param node The source YAML sequence node
	 * @param context The parser context for style detection
	 * @return Transform2D The decoded Transform2D object
	 * @throws YAMLException If the sequence length is incorrect or elements are invalid
	 */
	Transform2D decode_from_sequence(const ryml::ConstNodeRef &node, ParserContext *context) const;

	/**
	 * @brief Converter for Vector2 components.
	 *
	 * Used to encode/decode the Vector2 columns of the Transform2D.
	 */
	std::unique_ptr<Vector2VariantConverter> vec2_converter;
};

} // namespace godot

#endif // TRANSFORM2D_YAML_H

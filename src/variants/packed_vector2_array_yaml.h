/**
 * @file packed_vector2_array_yaml.h
 * @brief Defines the PackedVector2ArrayVariantConverter for YAML serialization of Godot PackedVector2Array type.
 *
 * This file contains the PackedVector2ArrayVariantConverter class which handles the conversion
 * between Godot PackedVector2Array objects and their YAML representation.
 */

#ifndef PACKED_VECTOR2_ARRAY_YAML_H
#define PACKED_VECTOR2_ARRAY_YAML_H

#include "../converter_factory.h"
#include "../variant_converter.h"
#include "vector2_yaml.h"

namespace godot {

/**
 * @class PackedVector2ArrayVariantConverter
 * @brief Converter for Godot PackedVector2Array type to/from YAML format.
 *
 * The PackedVector2ArrayVariantConverter class provides functionality to:
 * - Encode PackedVector2Array objects into YAML sequence nodes
 * - Decode YAML sequence nodes into PackedVector2Array objects
 * - Apply appropriate formatting based on style settings
 *
 * PackedVector2Array objects are represented in YAML as sequences of Vector2 values,
 * with each Vector2 formatted according to the style settings.
 *
 * @extends VariantConverter
 */
class PackedVector2ArrayVariantConverter : public VariantConverter {
public:
	/**
	 * @brief Constructor that initializes the converter.
	 *
	 * Initializes the Vector2 converter required for array element encoding.
	 *
	 * @param factory Pointer to the converter factory
	 */
	explicit PackedVector2ArrayVariantConverter(ConverterFactory *factory);

	/**
	 * @brief Defines the YAML tag information.
	 *
	 * - TAG: "PackedVector2Array" (used in YAML documents)
	 * - FULL_TAG: "!PackedVector2Array" (complete tag with prefix)
	 * - get_tag(): Returns the tag name
	 * - get_full_tag(): Returns the full tag
	 * - get_type(): Returns Variant::PACKED_VECTOR2_ARRAY
	 */
	DEFINE_YAML_TAG("PackedVector2Array", Variant::PACKED_VECTOR2_ARRAY)

	/**
	 * @brief Encodes a PackedVector2Array Variant to a YAML node.
	 *
	 * Converts a Godot PackedVector2Array to a YAML sequence node,
	 * with each Vector2 element formatted according to style settings.
	 * Supports template styling for consistent formatting of array elements.
	 *
	 * @param node The target YAML node
	 * @param v The PackedVector2Array Variant to encode
	 * @param style The style settings to apply
	 */
	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;

	/**
	 * @brief Decodes a YAML node to a PackedVector2Array Variant.
	 *
	 * Converts a YAML sequence node to a Godot PackedVector2Array,
	 * with each element converted to a Vector2 using the Vector2VariantConverter.
	 *
	 * @param node The source YAML node
	 * @param context The parser context for style detection
	 * @return Variant A PackedVector2Array Variant
	 * @throws YAMLException If the node is not a sequence or contains invalid elements
	 */
	Variant decode(const ryml::ConstNodeRef &node, ParserContext *context) const override;

private:
	/**
	 * @brief Converter for Vector2 elements.
	 *
	 * Used to encode/decode individual Vector2 elements in the array.
	 */
	std::unique_ptr<Vector2VariantConverter> vec2_converter;
};

} // namespace godot

#endif // PACKED_VECTOR2_ARRAY_YAML_H

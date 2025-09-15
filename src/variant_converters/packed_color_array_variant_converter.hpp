/**
 * @file packed_color_array_variant_converter.hpp
 * @brief Defines the YAMLPackedColorArrayVariantConverter for YAML serialization of Godot PackedColorArray type.
 *
 * This file contains the YAMLPackedColorArrayVariantConverter class which handles the conversion
 * between Godot PackedColorArray objects and their YAML representation.
 */
#pragma once

#include "color_variant_converter.hpp"
#include "variant_converter.hpp"
#include "variant_converter_factory.hpp"

namespace godot {

/**
 * @class YAMLPackedColorArrayVariantConverter
 * @brief Converter for Godot PackedColorArray type to/from YAML format.
 *
 * The YAMLPackedColorArrayVariantConverter class provides functionality to:
 * - Encode PackedColorArray objects into YAML sequence nodes
 * - Decode YAML sequence nodes into PackedColorArray objects
 * - Apply appropriate formatting based on style settings
 *
 * PackedColorArray objects are represented in YAML as sequences of Color values,
 * with each Color formatted according to the style settings (as maps, sequences, or HTML strings).
 *
 * @extends YAMLVariantConverter
 */
class YAMLPackedColorArrayVariantConverter : public YAMLVariantConverter {
public:
	/**
	 * @brief Constructor that initializes the converter.
	 *
	 * Initializes the Color converter required for array element encoding.
	 *
	 * @param factory Pointer to the converter factory
	 */
	explicit YAMLPackedColorArrayVariantConverter(YAMLVariantConverterFactory *factory);

	/**
	 * @brief Defines the YAML tag information.
	 *
	 * - TAG: "PackedColorArray" (used in YAML documents)
	 * - FULL_TAG: "!PackedColorArray" (complete tag with prefix)
	 * - get_tag(): Returns the tag name
	 * - get_full_tag(): Returns the full tag
	 * - get_type(): Returns Variant::PACKED_COLOR_ARRAY
	 */
	DEFINE_YAML_TAG("PackedColorArray", Variant::PACKED_COLOR_ARRAY)

	/**
	 * @brief Encodes a PackedColorArray Variant to a YAML node.
	 *
	 * Converts a Godot PackedColorArray to a YAML sequence node,
	 * with each Color element formatted according to style settings.
	 * Supports template styling for consistent formatting of array elements.
	 *
	 * @param node The target YAML node
	 * @param v The PackedColorArray Variant to encode
	 * @param style The style settings to apply
	 */
	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;

	/**
	 * @brief Decodes a YAML node to a PackedColorArray Variant.
	 *
	 * Converts a YAML sequence node to a Godot PackedColorArray,
	 * with each element converted to a Color using the YAMLColorVariantConverter.
	 *
	 * @param node The source YAML node
	 * @param context The parser context for style detection
	 * @return Variant A PackedColorArray Variant
	 * @throws YAMLException If the node is not a sequence or contains invalid elements
	 */
	Variant decode(const ryml::ConstNodeRef &node, YAMLParserContext *context) const override;

private:
	/**
	 * @brief Converter for Color elements.
	 *
	 * Used to encode/decode individual Color elements in the array.
	 */
	std::unique_ptr<YAMLColorVariantConverter> color_converter;
};

} // namespace godot

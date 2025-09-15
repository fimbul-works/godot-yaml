/**
 * @file packed_vector3_array_variant_converter.hpp
 * @brief Defines the YAMLPackedVector3ArrayVariantConverter for YAML serialization of Godot PackedVector3Array type.
 *
 * This file contains the YAMLPackedVector3ArrayVariantConverter class which handles the conversion
 * between Godot PackedVector3Array objects and their YAML representation.
 */
#pragma once

#include "variant_converter.hpp"
#include "variant_converter_factory.hpp"
#include "vector3_variant_converter.hpp"

namespace godot {

/**
 * @class YAMLPackedVector3ArrayVariantConverter
 * @brief Converter for Godot PackedVector3Array type to/from YAML format.
 *
 * The YAMLPackedVector3ArrayVariantConverter class provides functionality to:
 * - Encode PackedVector3Array objects into YAML sequence nodes
 * - Decode YAML sequence nodes into PackedVector3Array objects
 * - Apply appropriate formatting based on style settings
 *
 * PackedVector3Array objects are represented in YAML as sequences of Vector3 values,
 * with each Vector3 formatted according to the style settings.
 *
 * @extends YAMLVariantConverter
 */
class YAMLPackedVector3ArrayVariantConverter : public YAMLVariantConverter {
public:
	/**
	 * @brief Constructor that initializes the converter.
	 *
	 * Initializes the Vector3 converter required for array element encoding.
	 *
	 * @param factory Pointer to the converter factory
	 */
	explicit YAMLPackedVector3ArrayVariantConverter(YAMLVariantConverterFactory *factory);

	/**
	 * @brief Defines the YAML tag information.
	 *
	 * - TAG: "PackedVector3Array" (used in YAML documents)
	 * - FULL_TAG: "!PackedVector3Array" (complete tag with prefix)
	 * - get_tag(): Returns the tag name
	 * - get_full_tag(): Returns the full tag
	 * - get_type(): Returns Variant::PACKED_VECTOR3_ARRAY
	 */
	DEFINE_YAML_TAG("PackedVector3Array", Variant::PACKED_VECTOR3_ARRAY)

	/**
	 * @brief Encodes a PackedVector3Array Variant to a YAML node.
	 *
	 * Converts a Godot PackedVector3Array to a YAML sequence node,
	 * with each Vector3 element formatted according to style settings.
	 * Supports template styling for consistent formatting of array elements.
	 *
	 * @param node The target YAML node
	 * @param v The PackedVector3Array Variant to encode
	 * @param style The style settings to apply
	 */
	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;

	/**
	 * @brief Decodes a YAML node to a PackedVector3Array Variant.
	 *
	 * Converts a YAML sequence node to a Godot PackedVector3Array,
	 * with each element converted to a Vector3 using the YAMLVector3VariantConverter.
	 *
	 * @param node The source YAML node
	 * @param context The parser context for style detection
	 * @return Variant A PackedVector3Array Variant
	 * @throws YAMLException If the node is not a sequence or contains invalid elements
	 */
	Variant decode(const ryml::ConstNodeRef &node, YAMLParserContext *context) const override;

private:
	/**
	 * @brief Converter for Vector3 elements.
	 *
	 * Used to encode/decode individual Vector3 elements in the array.
	 */
	std::unique_ptr<YAMLVector3VariantConverter> vec3_converter;
};

} // namespace godot

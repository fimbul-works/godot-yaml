/**
 * @file packed_float32_array_variant_converter.hpp
 * @brief Defines the YAMLPackedFloat32ArrayVariantConverter for YAML serialization of Godot PackedFloat32Array type.
 *
 * This file contains the YAMLPackedFloat32ArrayVariantConverter class which handles the conversion
 * between Godot PackedFloat32Array objects and their YAML representation.
 */
#pragma once

#include "variant_converter.hpp"

namespace godot {

/**
 * @class YAMLPackedFloat32ArrayVariantConverter
 * @brief Converter for Godot PackedFloat32Array type to/from YAML format.
 *
 * The YAMLPackedFloat32ArrayVariantConverter class provides functionality to:
 * - Encode PackedFloat32Array objects into YAML sequence nodes
 * - Decode YAML sequence nodes into PackedFloat32Array objects
 * - Apply appropriate formatting based on style settings
 *
 * PackedFloat32Array objects are represented in YAML as sequences of floating-point values,
 * with optional style formatting for individual elements.
 *
 * @extends YAMLVariantConverter
 */
class YAMLPackedFloat32ArrayVariantConverter : public YAMLVariantConverter {
public:
	/**
	 * @brief Defines the YAML tag information.
	 *
	 * - TAG: "PackedFloat32Array" (used in YAML documents)
	 * - FULL_TAG: "!PackedFloat32Array" (complete tag with prefix)
	 * - get_tag(): Returns the tag name
	 * - get_full_tag(): Returns the full tag
	 * - get_type(): Returns Variant::PACKED_FLOAT32_ARRAY
	 */
	DEFINE_YAML_TAG("PackedFloat32Array", Variant::PACKED_FLOAT32_ARRAY)

	/**
	 * @brief Encodes a PackedFloat32Array Variant to a YAML node.
	 *
	 * Converts a Godot PackedFloat32Array to a YAML sequence node,
	 * with each float element formatted according to style settings.
	 * Supports template styling for consistent formatting of array elements.
	 *
	 * @param node The target YAML node
	 * @param v The PackedFloat32Array Variant to encode
	 * @param style The style settings to apply
	 */
	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;

	/**
	 * @brief Decodes a YAML node to a PackedFloat32Array Variant.
	 *
	 * Converts a YAML sequence node to a Godot PackedFloat32Array,
	 * with each element converted to a 32-bit floating-point value.
	 *
	 * @param node The source YAML node
	 * @param context The parser context for style detection
	 * @return Variant A PackedFloat32Array Variant
	 * @throws YAMLException If the node is not a sequence or contains invalid elements
	 */
	Variant decode(const ryml::ConstNodeRef &node, YAMLParserContext *context) const override;
};

} // namespace godot

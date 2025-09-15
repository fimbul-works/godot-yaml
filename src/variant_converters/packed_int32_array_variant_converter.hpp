/**
 * @file packed_int32_array_variant_converter.hpp
 * @brief Defines the YAMLPackedInt32ArrayVariantConverter for YAML serialization of Godot PackedInt32Array type.
 *
 * This file contains the YAMLPackedInt32ArrayVariantConverter class which handles the conversion
 * between Godot PackedInt32Array objects and their YAML representation.
 */
#pragma once

#include "variant_converter.hpp"

namespace godot {

/**
 * @class YAMLPackedInt32ArrayVariantConverter
 * @brief Converter for Godot PackedInt32Array type to/from YAML format.
 *
 * The YAMLPackedInt32ArrayVariantConverter class provides functionality to:
 * - Encode PackedInt32Array objects into YAML sequence nodes
 * - Decode YAML sequence nodes into PackedInt32Array objects
 * - Apply appropriate formatting based on style settings
 *
 * PackedInt32Array objects are represented in YAML as sequences of 32-bit integer values,
 * with optional style formatting for individual elements (decimal, hex, octal, or binary).
 *
 * @extends YAMLVariantConverter
 */
class YAMLPackedInt32ArrayVariantConverter : public YAMLVariantConverter {
public:
	/**
	 * @brief Defines the YAML tag information.
	 *
	 * - TAG: "PackedInt32Array" (used in YAML documents)
	 * - FULL_TAG: "!PackedInt32Array" (complete tag with prefix)
	 * - get_tag(): Returns the tag name
	 * - get_full_tag(): Returns the full tag
	 * - get_type(): Returns Variant::PACKED_INT32_ARRAY
	 */
	DEFINE_YAML_TAG("PackedInt32Array", Variant::PACKED_INT32_ARRAY)

	/**
	 * @brief Encodes a PackedInt32Array Variant to a YAML node.
	 *
	 * Converts a Godot PackedInt32Array to a YAML sequence node,
	 * with each integer element formatted according to style settings.
	 * Supports template styling for consistent formatting of array elements.
	 *
	 * @param node The target YAML node
	 * @param v The PackedInt32Array Variant to encode
	 * @param style The style settings to apply
	 */
	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;

	/**
	 * @brief Decodes a YAML node to a PackedInt32Array Variant.
	 *
	 * Converts a YAML sequence node to a Godot PackedInt32Array,
	 * with each element converted to a 32-bit integer value.
	 * Handles various integer formats (decimal, hex, octal, binary).
	 *
	 * @param node The source YAML node
	 * @param context The parser context for style detection
	 * @return Variant A PackedInt32Array Variant
	 * @throws YAMLException If the node is not a sequence, contains invalid elements,
	 *                      or contains values out of the int32 range
	 */
	Variant decode(const ryml::ConstNodeRef &node, YAMLParserContext *context) const override;
};

} // namespace godot

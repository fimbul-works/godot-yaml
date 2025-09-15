/**
 * @file packed_int64_array_variant_converter.hpp
 * @brief Defines the YAMLPackedInt64ArrayVariantConverter for YAML serialization of Godot PackedInt64Array type.
 *
 * This file contains the YAMLPackedInt64ArrayVariantConverter class which handles the conversion
 * between Godot PackedInt64Array objects and their YAML representation.
 */
#pragma once

#include "variant_converter.hpp"

namespace godot {

/**
 * @class YAMLPackedInt64ArrayVariantConverter
 * @brief Converter for Godot PackedInt64Array type to/from YAML format.
 *
 * The YAMLPackedInt64ArrayVariantConverter class provides functionality to:
 * - Encode PackedInt64Array objects into YAML sequence nodes
 * - Decode YAML sequence nodes into PackedInt64Array objects
 * - Apply appropriate formatting based on style settings
 *
 * PackedInt64Array objects are represented in YAML as sequences of 64-bit integer values,
 * with optional style formatting for individual elements (decimal, hex, octal, or binary).
 *
 * @extends YAMLVariantConverter
 */
class YAMLPackedInt64ArrayVariantConverter : public YAMLVariantConverter {
public:
	/**
	 * @brief Defines the YAML tag information.
	 *
	 * - TAG: "PackedInt64Array" (used in YAML documents)
	 * - FULL_TAG: "!PackedInt64Array" (complete tag with prefix)
	 * - get_tag(): Returns the tag name
	 * - get_full_tag(): Returns the full tag
	 * - get_type(): Returns Variant::PACKED_INT64_ARRAY
	 */
	DEFINE_YAML_TAG("PackedInt64Array", Variant::PACKED_INT64_ARRAY)

	/**
	 * @brief Encodes a PackedInt64Array Variant to a YAML node.
	 *
	 * Converts a Godot PackedInt64Array to a YAML sequence node,
	 * with each integer element formatted according to style settings.
	 * Supports template styling for consistent formatting of array elements.
	 *
	 * @param node The target YAML node
	 * @param v The PackedInt64Array Variant to encode
	 * @param style The style settings to apply
	 */
	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;

	/**
	 * @brief Decodes a YAML node to a PackedInt64Array Variant.
	 *
	 * Converts a YAML sequence node to a Godot PackedInt64Array,
	 * with each element converted to a 64-bit integer value.
	 * Handles various integer formats (decimal, hex, octal, binary).
	 *
	 * @param node The source YAML node
	 * @param context The parser context for style detection
	 * @return Variant A PackedInt64Array Variant
	 * @throws YAMLException If the node is not a sequence or contains invalid elements
	 */
	Variant decode(const ryml::ConstNodeRef &node, YAMLParserContext *context) const override;
};

} // namespace godot

/**
 * @file packed_string_array_variant_converter.hpp
 * @brief Defines the YAMLPackedStringArrayVariantConverter for YAML serialization of Godot PackedStringArray type.
 *
 * This file contains the YAMLPackedStringArrayVariantConverter class which handles the conversion
 * between Godot PackedStringArray objects and their YAML representation.
 */
#pragma once

#include "variant_converter.hpp"

namespace godot {

/**
 * @class YAMLPackedStringArrayVariantConverter
 * @brief Converter for Godot PackedStringArray type to/from YAML format.
 *
 * The YAMLPackedStringArrayVariantConverter class provides functionality to:
 * - Encode PackedStringArray objects into YAML sequence nodes
 * - Decode YAML sequence nodes into PackedStringArray objects
 * - Apply appropriate formatting based on style settings
 *
 * PackedStringArray objects are represented in YAML as sequences of string values,
 * with optional style formatting for individual elements (plain, quoted, literal, folded).
 *
 * @extends YAMLVariantConverter
 */
class YAMLPackedStringArrayVariantConverter : public YAMLVariantConverter {
public:
	/**
	 * @brief Defines the YAML tag information.
	 *
	 * - TAG: "PackedStringArray" (used in YAML documents)
	 * - FULL_TAG: "!PackedStringArray" (complete tag with prefix)
	 * - get_tag(): Returns the tag name
	 * - get_full_tag(): Returns the full tag
	 * - get_type(): Returns Variant::PACKED_STRING_ARRAY
	 */
	DEFINE_YAML_TAG("PackedStringArray", Variant::PACKED_STRING_ARRAY)

	/**
	 * @brief Encodes a PackedStringArray Variant to a YAML node.
	 *
	 * Converts a Godot PackedStringArray to a YAML sequence node,
	 * with each string element formatted according to style settings.
	 * Supports template styling for consistent formatting of array elements.
	 *
	 * @param node The target YAML node
	 * @param v The PackedStringArray Variant to encode
	 * @param style The style settings to apply
	 */
	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;

	/**
	 * @brief Decodes a YAML node to a PackedStringArray Variant.
	 *
	 * Converts a YAML sequence node to a Godot PackedStringArray,
	 * with each element converted to a String value.
	 * Handles various string formats (plain, quoted, etc.) and detects style information.
	 *
	 * @param node The source YAML node
	 * @param context The parser context for style detection
	 * @return Variant A PackedStringArray Variant
	 * @throws YAMLException If the node is not a sequence or contains invalid elements
	 */
	Variant decode(const ryml::ConstNodeRef &node, YAMLParserContext *context) const override;
};

} // namespace godot

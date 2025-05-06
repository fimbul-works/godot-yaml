/**
 * @file packed_float64_array_yaml.h
 * @brief Defines the PackedFloat64ArrayVariantConverter for YAML serialization of Godot PackedFloat64Array type.
 *
 * This file contains the PackedFloat64ArrayVariantConverter class which handles the conversion
 * between Godot PackedFloat64Array objects and their YAML representation.
 */

#ifndef PACKED_FLOAT64_ARRAY_YAML_H
#define PACKED_FLOAT64_ARRAY_YAML_H

#include "../variant_converter.h"

namespace godot {

/**
 * @class PackedFloat64ArrayVariantConverter
 * @brief Converter for Godot PackedFloat64Array type to/from YAML format.
 *
 * The PackedFloat64ArrayVariantConverter class provides functionality to:
 * - Encode PackedFloat64Array objects into YAML sequence nodes
 * - Decode YAML sequence nodes into PackedFloat64Array objects
 * - Apply appropriate formatting based on style settings
 *
 * PackedFloat64Array objects are represented in YAML as sequences of double-precision
 * floating-point values, with optional style formatting for individual elements.
 *
 * @extends VariantConverter
 */
class PackedFloat64ArrayVariantConverter : public VariantConverter {
public:
	/**
	 * @brief Defines the YAML tag information.
	 *
	 * - TAG: "PackedFloat64Array" (used in YAML documents)
	 * - FULL_TAG: "!PackedFloat64Array" (complete tag with prefix)
	 * - get_tag(): Returns the tag name
	 * - get_full_tag(): Returns the full tag
	 * - get_type(): Returns Variant::PACKED_FLOAT64_ARRAY
	 */
	DEFINE_YAML_TAG("PackedFloat64Array", Variant::PACKED_FLOAT64_ARRAY)

	/**
	 * @brief Encodes a PackedFloat64Array Variant to a YAML node.
	 *
	 * Converts a Godot PackedFloat64Array to a YAML sequence node,
	 * with each double-precision float element formatted according to style settings.
	 * Supports template styling for consistent formatting of array elements.
	 *
	 * @param node The target YAML node
	 * @param v The PackedFloat64Array Variant to encode
	 * @param style The style settings to apply
	 */
	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;

	/**
	 * @brief Decodes a YAML node to a PackedFloat64Array Variant.
	 *
	 * Converts a YAML sequence node to a Godot PackedFloat64Array,
	 * with each element converted to a 64-bit floating-point value.
	 *
	 * @param node The source YAML node
	 * @param context The parser context for style detection
	 * @return Variant A PackedFloat64Array Variant
	 * @throws YAMLException If the node is not a sequence or contains invalid elements
	 */
	Variant decode(const ryml::ConstNodeRef &node, ParserContext *context) const override;
};

} // namespace godot

#endif // PACKED_FLOAT64_ARRAY_YAML_H

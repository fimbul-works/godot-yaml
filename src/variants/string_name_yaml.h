/**
 * @file string_name_yaml.h
 * @brief Defines the StringNameVariantConverter for YAML serialization of Godot StringName type.
 *
 * This file contains the StringNameVariantConverter class which handles the conversion
 * between Godot StringName objects and their YAML representation.
 */

#ifndef STRING_NAME_YAML_H
#define STRING_NAME_YAML_H

#include "../variant_converter.h"

namespace godot {

/**
 * @class StringNameVariantConverter
 * @brief Converter for Godot StringName type to/from YAML format.
 *
 * The StringNameVariantConverter class provides functionality to:
 * - Encode StringName objects into YAML scalar nodes
 * - Decode YAML scalar nodes into StringName objects
 * - Apply appropriate formatting based on style settings
 *
 * @extends VariantConverter
 */
class StringNameVariantConverter : public VariantConverter {
public:
	/**
	 * @brief Defines the YAML tag information.
	 *
	 * - TAG: "StringName" (used in YAML documents)
	 * - FULL_TAG: "!StringName" (complete tag with prefix)
	 * - get_tag(): Returns the tag name
	 * - get_full_tag(): Returns the full tag
	 * - get_type(): Returns Variant::STRING_NAME
	 */
	DEFINE_YAML_TAG("StringName", Variant::STRING_NAME)

	/**
	 * @brief Encodes a StringName Variant to a YAML node.
	 *
	 * This method converts a Godot StringName to a YAML scalar node,
	 * applying string style settings (quotes, folding, etc.) as specified.
	 *
	 * @param node The target YAML node
	 * @param v The StringName Variant to encode
	 * @param style The style settings to apply
	 */
	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;

	/**
	 * @brief Decodes a YAML node to a StringName Variant.
	 *
	 * This method converts a YAML scalar node to a Godot StringName,
	 * handling string formatting and any necessary style detection.
	 *
	 * @param node The source YAML node
	 * @param context The parser context for style detection
	 * @return Variant A StringName Variant
	 * @throws YAMLException If the node is invalid or not a scalar
	 */
	Variant decode(const ryml::ConstNodeRef &node, ParserContext *context) const override;
};

} // namespace godot

#endif // STRING_NAME_YAML_H

/**
 * @file node_path_variant_converter.hpp
 * @brief Defines the YAMLNodePathVariantConverter for YAML serialization of Godot NodePath type.
 *
 * This file contains the YAMLNodePathVariantConverter class which handles the conversion
 * between Godot NodePath objects and their YAML representation.
 */
#pragma once

#include "variant_converter.hpp"

namespace godot {

/**
 * @class YAMLNodePathVariantConverter
 * @brief Converter for Godot NodePath type to/from YAML format.
 *
 * The YAMLNodePathVariantConverter class provides functionality to:
 * - Encode NodePath objects into YAML scalar nodes
 * - Decode YAML scalar nodes into NodePath objects
 * - Apply appropriate formatting based on style settings
 *
 * NodePath objects are represented in YAML as string scalar nodes,
 * containing the path string (e.g., "Parent/Child" or "../Sibling").
 *
 * @extends YAMLVariantConverter
 */
class YAMLNodePathVariantConverter : public YAMLVariantConverter {
public:
	/**
	 * @brief Defines the YAML tag information.
	 *
	 * - TAG: "NodePath" (used in YAML documents)
	 * - FULL_TAG: "!NodePath" (complete tag with prefix)
	 * - get_tag(): Returns the tag name
	 * - get_full_tag(): Returns the full tag
	 * - get_type(): Returns Variant::NODE_PATH
	 */
	DEFINE_YAML_TAG("NodePath", Variant::NODE_PATH)

	/**
	 * @brief Encodes a NodePath Variant to a YAML node.
	 *
	 * Converts a Godot NodePath to a YAML scalar node,
	 * applying string style settings (quotes, etc.) as specified.
	 *
	 * @param node The target YAML node
	 * @param v The NodePath Variant to encode
	 * @param style The style settings to apply
	 */
	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;

	/**
	 * @brief Decodes a YAML node to a NodePath Variant.
	 *
	 * Converts a YAML scalar node to a Godot NodePath,
	 * handling string formatting and any necessary style detection.
	 *
	 * @param node The source YAML node
	 * @param context The parser context for style detection
	 * @return Variant A NodePath Variant
	 * @throws YAMLException If the node is invalid or not a scalar
	 */
	Variant decode(const ryml::ConstNodeRef &node, YAMLParserContext *context) const override;
};

} // namespace godot

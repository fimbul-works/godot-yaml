/**
 * @file basis_variant_converter.hpp
 * @brief Defines the YAMLBasisVariantConverter for YAML serialization of Godot Basis type.
 *
 * This file contains the YAMLBasisVariantConverter class which handles the conversion
 * between Godot Basis objects and their YAML representation.
 */
#pragma once

#include "variant_converter.hpp"
#include "variant_converter_factory.hpp"
#include "vector3_variant_converter.hpp"

namespace godot {

/**
 * @class YAMLBasisVariantConverter
 * @brief Converter for Godot Basis type to/from YAML format.
 *
 * The YAMLBasisVariantConverter class provides functionality to:
 * - Encode Basis objects into YAML nodes (as maps or sequences)
 * - Decode YAML nodes into Basis objects
 * - Apply appropriate formatting based on style settings
 *
 * Basis objects (3x3 rotation/scale matrices) are represented in YAML either as:
 * - A map with "x", "y", and "z" fields, each containing Vector3 values representing columns
 * - A sequence of three Vector3 values representing the x, y, and z columns
 *
 * @extends YAMLVariantConverter
 */
class YAMLBasisVariantConverter : public YAMLVariantConverter {
public:
	/**
	 * @brief Constructor that initializes the converter.
	 *
	 * Initializes the Vector3 converter required for Basis column encoding.
	 *
	 * @param factory Pointer to the converter factory
	 */
	explicit YAMLBasisVariantConverter(YAMLVariantConverterFactory *factory);

	/**
	 * @brief Defines the YAML tag information.
	 *
	 * - TAG: "Basis" (used in YAML documents)
	 * - FULL_TAG: "!Basis" (complete tag with prefix)
	 * - get_tag(): Returns the tag name
	 * - get_full_tag(): Returns the full tag
	 * - get_type(): Returns Variant::BASIS
	 */
	DEFINE_YAML_TAG("Basis", Variant::BASIS)

	/**
	 * @brief Encodes a Basis Variant to a YAML node.
	 *
	 * Converts a Godot Basis to a YAML node, with format determined by style settings.
	 * Can produce either a map with x/y/z fields or a sequence of three Vector3 values.
	 *
	 * @param node The target YAML node
	 * @param v The Basis Variant to encode
	 * @param style The style settings to apply
	 */
	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;

	/**
	 * @brief Decodes a YAML node to a Basis Variant.
	 *
	 * Converts a YAML node to a Godot Basis, handling both map and sequence formats.
	 *
	 * @param node The source YAML node
	 * @param context The parser context for style detection
	 * @return Variant A Basis Variant
	 * @throws YAMLException If the node format is invalid or incompatible
	 */
	Variant decode(const ryml::ConstNodeRef &node, YAMLParserContext *context) const override;

private:
	/**
	 * @brief Emission helper methods.
	 */

	/**
	 * @brief Emits a Basis as a map.
	 *
	 * @param node The target YAML node
	 * @param basis The Basis to emit
	 * @param style The style settings to apply
	 */
	void emit_as_map(ryml::NodeRef &node, const Basis &basis, const YAMLStyle::View &style) const;

	/**
	 * @brief Emits a Basis as a sequence.
	 *
	 * @param node The target YAML node
	 * @param basis The Basis to emit
	 * @param style The style settings to apply
	 */
	void emit_as_sequence(ryml::NodeRef &node, const Basis &basis, const YAMLStyle::View &style) const;

	/**
	 * @brief Decodes a Basis from a map-formatted YAML node.
	 *
	 * Processes a YAML map with "x", "y", and "z" fields into a Basis.
	 *
	 * @param node The source YAML map node
	 * @param context The parser context for style detection
	 * @return Variant The decoded Basis object
	 * @throws YAMLException If required fields are missing or invalid
	 */
	Variant decode_from_map(const ryml::ConstNodeRef &node, YAMLParserContext *context) const;

	/**
	 * @brief Decodes a Basis from a sequence-formatted YAML node.
	 *
	 * Processes a YAML sequence of three Vector3 values into a Basis.
	 *
	 * @param node The source YAML sequence node
	 * @param context The parser context for style detection
	 * @return Variant The decoded Basis object
	 * @throws YAMLException If the sequence length is incorrect or elements are invalid
	 */
	Variant decode_from_sequence(const ryml::ConstNodeRef &node, YAMLParserContext *context) const;

	/**
	 * @brief Converter for Vector3 components.
	 *
	 * Used to encode/decode the column vectors of the Basis.
	 */
	std::unique_ptr<YAMLVector3VariantConverter> vec3_converter;
};

} // namespace godot

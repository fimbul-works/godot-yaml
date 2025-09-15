/**
 * @file transform3d_variant_converter.hpp
 * @brief Defines the YAMLTransform3DVariantConverter for YAML serialization of Godot Transform3D type.
 *
 * This file contains the YAMLTransform3DVariantConverter class which handles the conversion
 * between Godot Transform3D objects and their YAML representation.
 */
#pragma once

#include "basis_variant_converter.hpp"
#include "variant_converter.hpp"
#include "variant_converter_factory.hpp"
#include "vector3_variant_converter.hpp"

namespace godot {

/**
 * @class YAMLTransform3DVariantConverter
 * @brief Converter for Godot Transform3D type to/from YAML format.
 *
 * The YAMLTransform3DVariantConverter class provides functionality to:
 * - Encode Transform3D objects into YAML nodes (as maps or sequences)
 * - Decode YAML nodes into Transform3D objects
 * - Apply appropriate formatting based on style settings
 *
 * Transform3D objects are represented in YAML either as:
 * - A map with "basis" and "origin" fields
 * - A sequence of two elements (basis and origin)
 *
 * @extends YAMLVariantConverter
 */
class YAMLTransform3DVariantConverter : public YAMLVariantConverter {
public:
	/**
	 * @brief Constructor that initializes the converter.
	 *
	 * Initializes the Basis and Vector3 converters required for Transform3D components.
	 *
	 * @param factory Pointer to the converter factory
	 */
	explicit YAMLTransform3DVariantConverter(YAMLVariantConverterFactory *factory);

	/**
	 * @brief Defines the YAML tag information.
	 *
	 * - TAG: "Transform3D" (used in YAML documents)
	 * - FULL_TAG: "!Transform3D" (complete tag with prefix)
	 * - get_tag(): Returns the tag name
	 * - get_full_tag(): Returns the full tag
	 * - get_type(): Returns Variant::TRANSFORM3D
	 */
	DEFINE_YAML_TAG("Transform3D", Variant::TRANSFORM3D)

	/**
	 * @brief Encodes a Transform3D Variant to a YAML node.
	 *
	 * Converts a Godot Transform3D to a YAML node, with format determined by style settings.
	 * Can produce either a map with named fields or a sequence of values.
	 *
	 * @param node The target YAML node
	 * @param v The Transform3D Variant to encode
	 * @param style The style settings to apply
	 */
	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;

	/**
	 * @brief Decodes a YAML node to a Transform3D Variant.
	 *
	 * Converts a YAML node to a Godot Transform3D, handling both map and sequence formats.
	 *
	 * @param node The source YAML node
	 * @param context The parser context for style detection
	 * @return Variant A Transform3D Variant
	 * @throws YAMLException If the node format is invalid or incompatible
	 */
	Variant decode(const ryml::ConstNodeRef &node, YAMLParserContext *context) const override;

private:
	/**
	 * @brief Decodes a Transform3D from a map-formatted YAML node.
	 *
	 * Processes a YAML map with "basis" and "origin" fields into a Transform3D.
	 *
	 * @param node The source YAML map node
	 * @param context The parser context for style detection
	 * @return Transform3D The decoded Transform3D object
	 * @throws YAMLException If required fields are missing or invalid
	 */
	Transform3D decode_from_map(const ryml::ConstNodeRef &node, YAMLParserContext *context) const;

	/**
	 * @brief Decodes a Transform3D from a sequence-formatted YAML node.
	 *
	 * Processes a YAML sequence containing basis and origin values into a Transform3D.
	 *
	 * @param node The source YAML sequence node
	 * @param context The parser context for style detection
	 * @return Transform3D The decoded Transform3D object
	 * @throws YAMLException If the sequence length is incorrect or elements are invalid
	 */
	Transform3D decode_from_sequence(const ryml::ConstNodeRef &node, YAMLParserContext *context) const;

	/**
	 * @brief Converter for Basis component.
	 *
	 * Used to encode/decode the Basis part of the Transform3D.
	 */
	std::unique_ptr<YAMLBasisVariantConverter> basis_converter;

	/**
	 * @brief Converter for Vector3 component.
	 *
	 * Used to encode/decode the origin (Vector3) part of the Transform3D.
	 */
	std::unique_ptr<YAMLVector3VariantConverter> vec3_converter;
};

} // namespace godot

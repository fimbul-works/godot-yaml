/**
 * @file plane_yaml.h
 * @brief Defines the PlaneVariantConverter for YAML serialization of Godot Plane type.
 *
 * This file contains the PlaneVariantConverter class which handles the conversion
 * between Godot Plane objects and their YAML representation.
 */

#ifndef PLANE_YAML_H
#define PLANE_YAML_H

#include "../converter_factory.h"
#include "../variant_converter.h"
#include "vector3_yaml.h"

namespace godot {

/**
 * @class PlaneVariantConverter
 * @brief Converter for Godot Plane type to/from YAML format.
 *
 * The PlaneVariantConverter class provides functionality to:
 * - Encode Plane objects into YAML nodes (as maps or sequences)
 * - Decode YAML nodes into Plane objects
 * - Apply appropriate formatting based on style settings
 *
 * Plane objects are represented in YAML either as:
 * - A map with "normal" (Vector3) and "d" (distance) fields
 * - A sequence with two elements: normal vector and distance value
 *
 * @extends VariantConverter
 */
class PlaneVariantConverter : public VariantConverter {
public:
	/**
	 * @brief Constructor that initializes the converter.
	 *
	 * Initializes the Vector3 converter required for the normal component.
	 *
	 * @param factory Pointer to the converter factory
	 */
	explicit PlaneVariantConverter(ConverterFactory *factory);

	/**
	 * @brief Defines the YAML tag information.
	 *
	 * - TAG: "Plane" (used in YAML documents)
	 * - FULL_TAG: "!Plane" (complete tag with prefix)
	 * - get_tag(): Returns the tag name
	 * - get_full_tag(): Returns the full tag
	 * - get_type(): Returns Variant::PLANE
	 */
	DEFINE_YAML_TAG("Plane", Variant::PLANE)

	/**
	 * @brief Encodes a Plane Variant to a YAML node.
	 *
	 * Converts a Godot Plane to a YAML node, with format determined by style settings.
	 * Can produce either a map or a sequence representation.
	 *
	 * @param node The target YAML node
	 * @param v The Plane Variant to encode
	 * @param style The style settings to apply
	 */
	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;

	/**
	 * @brief Decodes a YAML node to a Plane Variant.
	 *
	 * Converts a YAML node to a Godot Plane, handling both map and sequence formats.
	 *
	 * @param node The source YAML node
	 * @param context The parser context for style detection
	 * @return Variant A Plane Variant
	 * @throws YAMLException If the node format is invalid or incompatible
	 */
	Variant decode(const ryml::ConstNodeRef &node, ParserContext *context) const override;

private:
	/**
	 * @brief Emission helper methods.
	 */

	/**
	 * @brief Emits a Plane as a map.
	 *
	 * @param node The target YAML node
	 * @param plane The Plane to emit
	 * @param style The style settings to apply
	 */
	void emit_as_map(ryml::NodeRef &node, const Plane &plane, const YAMLStyle::View &style) const;

	/**
	 * @brief Emits a Plane as a sequence.
	 *
	 * @param node The target YAML node
	 * @param plane The Plane to emit
	 * @param style The style settings to apply
	 */
	void emit_as_sequence(ryml::NodeRef &node, const Plane &plane, const YAMLStyle::View &style) const;

	/**
	 * @brief Decodes a Plane from a map-formatted YAML node.
	 *
	 * Processes a YAML map with "normal" and "d" fields into a Plane.
	 *
	 * @param node The source YAML map node
	 * @param context The parser context for style detection
	 * @return Variant The decoded Plane object
	 * @throws YAMLException If required fields are missing or invalid
	 */
	Variant decode_from_map(const ryml::ConstNodeRef &node, ParserContext *context) const;

	/**
	 * @brief Decodes a Plane from a sequence-formatted YAML node.
	 *
	 * Processes a YAML sequence of normal vector and distance value into a Plane.
	 *
	 * @param node The source YAML sequence node
	 * @param context The parser context for style detection
	 * @return Variant The decoded Plane object
	 * @throws YAMLException If the sequence length is incorrect or elements are invalid
	 */
	Variant decode_from_sequence(const ryml::ConstNodeRef &node, ParserContext *context) const;

	/**
	 * @brief Converter for Vector3 normal component.
	 *
	 * Used to encode/decode the normal vector of the Plane.
	 */
	std::unique_ptr<Vector3VariantConverter> vec3_converter;
};

} // namespace godot

#endif // PLANE_YAML_H

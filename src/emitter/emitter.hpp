/**
 * @file emitter.hpp
 * @brief Defines the YAMLEmitter class for serializing Godot types to YAML.
 *
 * This file contains the YAMLEmitter class which handles the conversion of
 * Godot Variant types to YAML format. It supports customizable formatting
 * through style options and manages the serialization process.
 */
#pragma once

#include "../style/style_view.hpp"
#include "../util/string_hash.hpp"
#include "../util/string_pool.hpp"
#include "../variant_converters/variant_converter_factory.hpp"
#include "../yaml.hpp"
#include "result.hpp"

#include <godot_cpp/classes/resource.hpp>
#include <ryml.hpp>

#include <memory>
#include <mutex>

namespace godot {

/**
 * @class YAMLEmitter
 * @brief Converts Godot Variants to YAML format.
 *
 * The YAMLEmitter class handles the serialization of Godot Variant types to YAML
 * format. It supports various Godot types, including primitive types,
 * containers, objects, and resources. It also provides style customization
 * through the YAMLStyle system.
 *
 * Each Godot type is handled by a specialized YAMLVariantConverter, which is
 * managed by the YAMLVariantConverterFactory.
 */
class YAML::Emitter {
public:
	/**
	 * @brief Constructs a new YAMLEmitter instance.
	 *
	 * Initializes the ryml components and type converters.
	 */
	Emitter();

	/**
	 * @brief Default destructor.
	 */
	~Emitter() = default;

	/**
	 * @brief Non-copyable class.
	 */
	YAML::Emitter(const Emitter &) = delete;
	YAML::Emitter &operator=(const Emitter &) = delete;

	/**
	 * @brief Emits a Godot Variant as YAML.
	 *
	 * @param input The Variant to serialize
	 * @param style Style settings for formatting
	 * @return Ref<YAMLResult> Result containing the YAML string or error
	 */
	Ref<YAMLResult> emit(const Variant &input, const YAMLStyle::View &style);

private:
	/**
	 * @brief Ryml setup components.
	 */
	ryml::Callbacks callbacks;
	std::unique_ptr<ryml::EventHandlerTree> evt_handler;
	ryml::Tree tree;

	/**
	 * @brief Converter components.
	 */
	YAMLVariantConverterFactory factory;
	std::unordered_map<Variant::Type, std::unique_ptr<YAMLVariantConverter>> type_converters;
	std::unordered_map<String, YAMLVariantConverter *, StringHasher, StringEqual> tag_converters;

	/**
	 * @brief Current emission state.
	 */
	Ref<YAMLResult> current_result;

	/**
	 * @brief Error callback for ryml.
	 *
	 * @param msg Error message
	 * @param len Length of error message
	 * @param loc Location in the document
	 * @param user_data User data pointer (typically the YAMLEmitter instance)
	 */
	static void error_callback(const char *msg, size_t len, ryml::Location loc, void *user_data);

	/**
	 * @brief Initializes the type converters.
	 */
	void init_converters();

	/**
	 * @brief Converter lookup methods.
	 */

	/**
	 * @brief Gets a converter for a specific Variant type.
	 *
	 * @param type The Variant type
	 * @return YAMLVariantConverter* The converter or nullptr if not found
	 */
	YAMLVariantConverter *get_converter_for_type(Variant::Type type) const;

	/**
	 * @brief Gets a converter for a specific YAML tag.
	 *
	 * @param tag The tag name
	 * @return YAMLVariantConverter* The converter or nullptr if not found
	 */
	YAMLVariantConverter *get_converter_for_tag(const String &tag) const;

	/**
	 * @brief String pool for efficient string handling.
	 */
	YAMLStringPool string_pool;

	/**
	 * @brief Stores a string in the string pool.
	 *
	 * @param str The string to store
	 * @return ryml::csubstr View of the stored string
	 */
	ryml::csubstr store_string(const String &str);

	/**
	 * @brief Core emission methods for different Godot types.
	 */

	/**
	 * @brief Main entry point for emitting any Variant type.
	 *
	 * @param node The target YAML node
	 * @param value The Variant to emit
	 * @param style Style settings for formatting
	 */
	void emit_value(ryml::NodeRef node, const Variant &value, const YAMLStyle::View &style);

	/**
	 * @brief Emits a null value.
	 *
	 * @param node The target YAML node
	 */
	void emit_nil(ryml::NodeRef &node);

	/**
	 * @brief Emits a boolean value.
	 *
	 * @param node The target YAML node
	 * @param value The boolean value
	 */
	void emit_bool(ryml::NodeRef &node, bool value);

	/**
	 * @brief Emits a numeric value (int or float).
	 *
	 * @param node The target YAML node
	 * @param value The numeric value
	 * @param style Style settings for formatting
	 */
	void emit_number(ryml::NodeRef &node, const Variant &value, const YAMLStyle::View &style);

	/**
	 * @brief Emits a string value.
	 *
	 * @param node The target YAML node
	 * @param value The string value
	 * @param style Style settings for formatting
	 */
	void emit_string(ryml::NodeRef &node, const String &value, const YAMLStyle::View &style);

	/**
	 * @brief Emits an array.
	 *
	 * @param node The target YAML node
	 * @param array The array to emit
	 * @param style Style settings for formatting
	 */
	void emit_array(ryml::NodeRef &node, const Array &array, const YAMLStyle::View &style);

	/**
	 * @brief Emits a dictionary.
	 *
	 * @param node The target YAML node
	 * @param dict The dictionary to emit
	 * @param style Style settings for formatting
	 */
	void emit_dictionary(ryml::NodeRef &node, const Dictionary &dict, const YAMLStyle::View &style);

	/**
	 * @brief Emits an object.
	 *
	 * @param node The target YAML node
	 * @param obj The object to emit
	 * @param style Style settings for formatting
	 */
	void emit_object(ryml::NodeRef &node, Object *obj, const YAMLStyle::View &style);

	/**
	 * @brief Emits a resource.
	 *
	 * @param node The target YAML node
	 * @param res The resource to emit
	 * @param style Style settings for formatting
	 */
	void emit_resource(ryml::NodeRef &node, const Resource *res, const YAMLStyle::View &style);

	/**
	 * @brief Maximum nesting depth to prevent stack overflows.
	 */
	static const int MAX_DEPTH = 100;

	/**
	 * @brief Checks if the current nesting depth exceeds the maximum.
	 *
	 * @param current_depth The current depth
	 * @throws YAMLException If the depth exceeds the maximum
	 */
	void check_depth(int current_depth);
};

} // namespace godot

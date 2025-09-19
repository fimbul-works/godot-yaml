/**
 * @file yaml.hpp
 * @brief Main interface for YAML parsing, serialization, and manipulation in Godot.
 *
 * This file defines the main YAML class which serves as the primary API for
 * working with YAML data in Godot. It provides functionality for parsing YAML
 * strings into Godot Variants, serializing Variants to YAML, and handling
 * YAML-specific settings like style and security.
 *
 * The class uses a modular architecture with specialized components for
 * parsing, emitting, and validation.
 */
#pragma once

#include "parser/security.hpp"
#include "result.hpp"
#include "style/style.hpp"

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/script.hpp>
#include <godot_cpp/variant/variant.hpp>

namespace godot {

/**
 * @class YAML
 * @brief Main class for YAML operations in Godot.
 *
 * The YAML class provides static methods for parsing, serializing, and
 * validating YAML content. It serves as the primary interface for all
 * YAML-related operations.
 *
 * Key features:
 * - Parsing YAML strings to Godot Variants
 * - Serializing Godot Variants to YAML strings
 * - Loading/saving YAML from/to files
 * - Style customization for YAML output
 * - Security features for safe resource loading
 * - Custom class registration for extended serialization capabilities
 *
 * @extends RefCounted
 */
class YAML : public RefCounted {
	GDCLASS(YAML, RefCounted);

protected:
	/**
	 * @brief Binds methods to make them accessible from GDScript.
	 */
	static void _bind_methods();

public:
	// Forward-declare inner classes
	class Emitter;
	class Parser;
	class Validator;
	class FileSaver;
	class FileLoader;
	class FileValidator;

	/**
	 * @brief Returns the version string of the YAML module.
	 *
	 * @return String The version information, including build target.
	 */
	static String version();

	/**
	 * @brief Parses a YAML string into a Godot Variant.
	 *
	 * @param input The YAML string to parse.
	 * @param security Optional security settings for resource loading.
	 * @param detect_style Whether to detect and store style information.
	 * @return Ref<YAMLResult> Result object containing parsed data or error information.
	 */
	static Ref<YAMLResult> parse(const String &input, const Ref<YAMLSecurity> security = nullptr, const bool detect_style = false);

	/**
	 * @brief Serializes a Godot Variant into a YAML string.
	 *
	 * @param input The Variant to convert to YAML.
	 * @param format Optional style settings for formatting the output.
	 * @return Ref<YAMLResult> Result object containing the YAML string or error information.
	 */
	static Ref<YAMLResult> stringify(const Variant &input, const Ref<YAMLStyle> &format = nullptr);

	/**
	 * @brief Validates a YAML string without fully parsing it.
	 *
	 * @param input The YAML string to validate.
	 * @return Ref<YAMLResult> Result object indicating whether the YAML is valid.
	 */
	static Ref<YAMLResult> validate(const String &input);

	/**
	 * @brief Loads and parses a YAML file.
	 *
	 * @param path Path to the YAML file.
	 * @param security Optional security settings for resource loading.
	 * @param detect_style Whether to detect and store style information.
	 * @return Ref<YAMLResult> Result object containing parsed data or error information.
	 */
	static Ref<YAMLResult> load_file(const String &path, const Ref<YAMLSecurity> security = nullptr, const bool detect_style = false);

	/**
	 * @brief Loads and parses a YAML file inside an existing parser context.
	 *
	 * @param path Path to the YAML file.
	 * @param security Optional security settings for resource loading.
	 * @return Ref<YAMLResult> Result object containing parsed data or error information.
	 */
	static Ref<YAMLResult> parser_load_file(const String &path, const YAMLSecurity::View &security_view, std::unordered_set<String, StringHasher, StringEqual> *loading_yaml_paths);

	/**
	 * @brief Serializes a Godot Variant and saves it to a YAML file.
	 *
	 * @param data The Variant to convert to YAML.
	 * @param path Path where the YAML file will be saved.
	 * @param style Optional style settings for formatting the output.
	 * @return Ref<YAMLResult> Result object indicating success or failure.
	 */
	static Ref<YAMLResult> save_file(const Variant &data, const String &path, const Ref<YAMLStyle> &style = nullptr);

	/**
	 * @brief Validates a YAML file without fully parsing it.
	 *
	 * @param path Path to the YAML file.
	 * @return Ref<YAMLResult> Result object indicating whether the YAML is valid.
	 */
	static Ref<YAMLResult> validate_file(const String &path);

	/**
	 * @brief Parses a YAML string, pushing errors to Godot's error system.
	 *
	 * This is a simplified version of parse() that returns null on error and
	 * automatically reports errors through Godot's error reporting system.
	 *
	 * @param input The YAML string to parse.
	 * @param security Optional security settings for resource loading.
	 * @return Variant The parsed data or null if parsing failed.
	 */
	static Variant try_parse(const String &input, const Ref<YAMLSecurity> security = nullptr);

	/**
	 * @brief Serializes a Godot Variant to YAML, pushing errors to Godot's error system.
	 *
	 * This is a simplified version of stringify() that returns an empty string on error
	 * and automatically reports errors through Godot's error reporting system.
	 *
	 * @param input The Variant to convert to YAML.
	 * @param format Optional style settings for formatting the output.
	 * @return String The YAML string or empty string if serialization failed.
	 */
	static String try_stringify(const Variant &input, const Ref<YAMLStyle> &format = nullptr);

	/**
	 * @brief Loads and parses a YAML file, pushing errors to Godot's error system.
	 *
	 * This is a simplified version of load_file() that returns null on error and
	 * automatically reports errors through Godot's error reporting system.
	 *
	 * @param path Path to the YAML file.
	 * @param security Optional security settings for resource loading.
	 * @return Variant The parsed data or null if loading failed.
	 */
	static Variant try_load_file(const String &path, const Ref<YAMLSecurity> security = nullptr);

	/**
	 * @brief Serializes and saves a Godot Variant to a YAML file, pushing errors to Godot's error system.
	 *
	 * This is a simplified version of save_file() that returns a boolean success indicator and
	 * automatically reports errors through Godot's error reporting system.
	 *
	 * @param data The Variant to convert to YAML.
	 * @param path Path where the YAML file will be saved.
	 * @param style Optional style settings for formatting the output.
	 * @return bool True if saving succeeded, false otherwise.
	 */
	static bool try_save_file(const Variant &data, const String &path, const Ref<YAMLStyle> &style = nullptr);

	/**
	 * @brief Registers a custom class for YAML serialization/deserialization.
	 *
	 * This allows custom GDScript classes to be serialized to YAML and deserialized from YAML
	 * using specified methods.
	 *
	 * @param p_class The Script resource representing the class.
	 * @param p_serialize_method The name of the instance method used for serialization.
	 * @param p_deserialize_method The name of the static method used for deserialization.
	 */
	static void register_class(Ref<Script> p_class, const Variant &p_serialize_method, const Variant &p_deserialize_method);

	/**
	 * @brief Unregisters a custom class from YAML serialization.
	 *
	 * @param p_class The Script resource representing the class to unregister.
	 */
	static void unregister_class(Ref<Script> p_class);

	/**
	 * @brief Checks if a class is registered for YAML serialization.
	 *
	 * @param class_name The name of the class.
	 * @return bool True if the class is registered, false otherwise.
	 */
	static bool has_registered_class(const String &class_name);

	/**
	 * @brief Creates a new YAMLStyle instance for customizing YAML output.
	 *
	 * @return Ref<YAMLStyle> A new style object.
	 */
	static Ref<YAMLStyle> create_style();

	/**
	 * @brief Creates a new YAMLSecurity instance for resource loading security.
	 *
	 * @return Ref<YAMLSecurity> A new security object.
	 */
	static Ref<YAMLSecurity> create_security();

	/**
	 * @brief Allows resource loading from a specific path prefix.
	 *
	 * @param path_prefix The path prefix to allow.
	 * @param type_names Optional array of allowed resource types for this path.
	 */
	static void allow_resource_path(const String &path_prefix, const Array &type_names = Array());

	/**
	 * @brief Blocks a specific resource type from being loaded.
	 *
	 * @param type_name The resource type to block.
	 */
	static void block_resource_type(const StringName &type_name);

	/**
	 * @brief Resets security settings to default values.
	 */
	static void reset_security();

	/**
	 * @brief Converts the YAML object to a string representation.
	 *
	 * @return String String representation of the YAML object.
	 */
	String _to_string() const;
};

} // namespace godot

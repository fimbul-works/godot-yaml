/**
 * @file file_validator.hpp
 * @brief Defines the YAMLFileValidator class for validating YAML files.
 *
 * This file contains the YAMLFileValidator class which provides static methods
 * for validating YAML files and strings. It uses a singleton pattern to support
 * future features like JSON Schema validation and schema registration.
 */
#pragma once

#include "../result.hpp"
#include "../util/hashers.hpp"
#include "../validator/validator.hpp"
#include "../yaml.hpp"

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <mutex>
#include <unordered_map>

namespace godot {

/**
 * @class YAML::FileValidator
 * @brief Provides static methods for validating YAML files and managing schemas.
 *
 * The YAMLFileValidator class uses a singleton pattern to provide validation
 * services and future schema management capabilities. It offers static methods
 * for validating YAML files and strings without the overhead of full parsing.
 */
class YAML::FileValidator {
public:
	/**
	 * @brief Gets the singleton instance.
	 *
	 * @return FileValidator* Pointer to the singleton instance
	 */
	static FileValidator *get_singleton();

	/**
	 * @brief Validates a YAML file without full parsing.
	 *
	 * @param path Path to the YAML file to validate
	 * @return Ref<YAMLResult> Result object indicating validation success or failure
	 */
	static Ref<YAMLResult> validate_file(const String &path);

	/**
	 * @brief Validates a YAML string without full parsing.
	 *
	 * @param input The YAML string to validate
	 * @return Ref<YAMLResult> Result object indicating validation success or failure
	 */
	static Ref<YAMLResult> validate_string(const String &input);

private:
	/**
	 * @brief Private constructor for singleton pattern.
	 */
	FileValidator();

	/**
	 * @brief Private destructor.
	 */
	~FileValidator() = default;

	/**
	 * @brief Non-copyable class.
	 */
	FileValidator(const FileValidator &) = delete;
	FileValidator &operator=(const FileValidator &) = delete;

	/**
	 * @brief Reads file content from disk with proper error handling.
	 *
	 * @param path Path to the file
	 * @param[out] content The file content if successful
	 * @return Ref<YAMLResult> Success result or error information
	 */
	Ref<YAMLResult> read_file_content(const String &path, String &content);
};

} //namespace godot

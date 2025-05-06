/**
 * @file validator.h
 * @brief Defines the YAML::Validator class for lightweight YAML validation.
 *
 * This file contains the YAML::Validator class which performs basic validation
 * of YAML input without the full parsing overhead. It checks for syntax errors
 * and structural issues without converting to Godot types.
 */

#ifndef YAML_VALIDATOR_H
#define YAML_VALIDATOR_H

#include "exception.h"
#include "result.h"
#include "yaml.h"
#include <memory>
#include <ryml.hpp>

namespace godot {

/**
 * @class YAML::Validator
 * @brief Validates YAML input for correct syntax and structure.
 *
 * The Validator class provides a lightweight mechanism to check if a YAML
 * string is valid without the overhead of full parsing and type conversion.
 * It is useful for quickly validating user input or checking YAML files
 * before attempting to parse them.
 *
 * The validator uses ryml (RapidYAML) for low-level validation and provides
 * detailed error information when validation fails.
 */
class YAML::Validator {
public:
	/**
	 * @brief Constructs a new Validator instance.
	 *
	 * Initializes the ryml parser and callbacks for validation.
	 */
	Validator();

	/**
	 * @brief Default destructor.
	 */
	~Validator() = default;

	/**
	 * @brief Non-copyable class.
	 */
	Validator(const Validator &) = delete;
	Validator &operator=(const Validator &) = delete;

	/**
	 * @brief Validates a YAML string without full parsing.
	 *
	 * Performs a lightweight check to determine if the YAML is syntactically
	 * valid without converting it to Godot types.
	 *
	 * @param input The YAML string to validate
	 * @return Ref<YAMLResult> Result object indicating validation success or failure
	 */
	Ref<YAMLResult> validate(const String &input);

private:
	/**
	 * @brief Internal ryml components for validation.
	 */
	ryml::Tree tree;
	ryml::Callbacks callbacks;
	std::unique_ptr<ryml::EventHandlerTree> evt_handler;
	std::unique_ptr<ryml::Parser> ryml_parser;

	/**
	 * @brief Result of the current validation operation.
	 */
	Ref<YAMLResult> current_result;

	/**
	 * @brief Callback for error handling during validation.
	 *
	 * @param msg Error message
	 * @param len Length of error message
	 * @param loc Location in the source document
	 * @param user_data User data pointer (typically the Validator instance)
	 */
	static void error_callback(const char *msg, size_t len, ryml::Location loc, void *user_data);
};

} // namespace godot

#endif // YAML_VALIDATOR_H

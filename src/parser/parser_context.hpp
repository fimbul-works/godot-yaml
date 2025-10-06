/**
 * @file parser_context.hpp
 * @brief Defines the YAMLParserContext class for YAML parsing state management.
 *
 * This file contains the YAMLParserContext class which maintains state during YAML
 * parsing operations, specifically for managing style information and providing
 * access to the underlying ryml parser instance.
 */
#pragma once

#include "../style/style.hpp"
#include "exception.hpp"
#include <schema.hpp>
#include <validation_error.hpp>
#include <validation_result.hpp>

#include <ryml.hpp>

#include <memory>
#include <stack>

namespace godot {

/**
 * @class YAMLParserContext
 * @brief Maintains parsing context during YAML processing.
 *
 * The YAMLParserContext class manages state during YAML parsing, particularly
 * related to style information. It maintains a stack of YAMLStyle objects
 * that track the style attributes for each level of the YAML hierarchy.
 * This allows the parser to preserve formatting information from the input.
 */
class YAMLParserContext {
public:
	/**
	 * @brief Constructs a new YAMLParserContext instance.
	 *
	 * @param p_ryml_parser Pointer to the underlying ryml parser instance
	 * @param root_style Optional root style object for style detection
	 * @param root_schema Optional root schema for validation
	 */
	YAMLParserContext(const ryml::Parser *p_ryml_parser, const Ref<YAMLStyle> &root_style = nullptr, const Ref<Schema> &root_schema = nullptr);

	/**
	 * @brief Default destructor.
	 */
	~YAMLParserContext() = default;

	/**
	 * @brief Gets the current style from the top of the style stack.
	 * @return Ref<YAMLStyle> The current style object
	 * @throws YAMLException If the style stack is empty or invalid
	 */
	Ref<YAMLStyle> current_style() const;

	/**
	 * @brief Pushes a new style onto the stack for a child element.
	 *
	 * Creates a new style object, adds it as a child to the current style,
	 * and pushes it onto the stack to become the current style.
	 *
	 * @param key The key or identifier for the child element
	 * @return Ref<YAMLStyle> The newly created style object
	 * @throws YAMLException If the parent style is invalid
	 */
	Ref<YAMLStyle> push_style(const String &key);

	/**
	 * @brief Pops the top style from the stack.
	 *
	 * Removes the current style from the stack, making the parent style
	 * the new current style.
	 */
	void pop_style();

	/**
	 * @brief Checks if schema validation is currently active.
	 * @return true if schema validation is active, false otherwise
	 */
	bool is_validating() const { return !schema_stack.empty(); }

	/**
	 * @brief Checks if schema discovery is needed (no root schema set).
	 * @return true if schema discovery is needed, false otherwise
	 */
	bool needs_schema_discovery() const { return !has_root_schema; }

	/**
	 * @brief Sets the root schema for validation.
	 *
	 * Initializes the schema stack with the provided root schema.
	 * If a root schema is already set, this call is ignored.
	 *
	 * @param schema The root schema to set
	 */
	void set_root_schema(const Ref<Schema> &schema) {
		if (schema.is_valid() && schema_stack.empty()) {
			schema_stack.push(schema);
			has_root_schema = true;
		}
	}

	// Schema tree traversal (parallel to YAML tree)

	/**
	 * @brief Resolves a schema reference if present
	 * @param schema The schema to potentially resolve
	 * @return Resolved schema or original if not a reference
	 */
	Ref<Schema> resolve_schema_reference(const Ref<Schema> &schema) const;

	/**
	 * @brief Pushes the schema for a property onto the schema stack.
	 * @param property_name The name of the property to push the schema for
	 */
	void push_schema_for_property(const String &property_name);

	/**
	 * @brief Pushes the schema for an array item onto the schema stack.
	 * @param index The index of the array item (for error reporting)
	 */
	void push_schema_for_array_item(int index);

	/**
	 * @brief Pushes a uniform schema for all array items onto the schema stack.
	 */
	void push_schema_for_array_items(); // Uniform schema for all items

	/**
	 * @brief Pops the top schema from the schema stack.
	 */
	void pop_schema();

	/**
	 * @brief Gets the current schema from the top of the schema stack.
	 * @return Current schema for validation, or nullptr if none
	 */
	Ref<Schema> current_schema() const;

	/**
	 * @brief Adds a validation error to the context.
	 * @param error The YAMLValidationError to add
	 */
	void add_validation_error(const ValidationError &error);

	/**
	 * @brief Retrieves the list of validation errors.
	 * @return const std::vector<YAMLValidationError>& Reference to the list of errors
	 */
	const std::vector<ValidationError> &get_validation_errors() const { return validation_errors; }

	/**
	 * @brief Checks if there are any validation errors.
	 * @return true if there are validation errors, false otherwise
	 */
	bool has_validation_errors() const { return !validation_errors.empty(); }

	// Path tracking (for nested resources)

	/**
	 * @brief Gets the current instance path as a string.
	 * @return String The current instance path (e.g., "/root/child/item")
	 */
	String get_current_instance_path() const;

	/**
	 * @brief Gets the current instance path as an array of segments.
	 * @return An array of strings representing the path segments
	 */
	Array get_current_instance_path_array() const {
		Array path_array;
		for (const String &segment : instance_path_segments) {
			path_array.append(segment);
		}
		return path_array;
	}

	/**
	 * @brief Pushes a segment onto the instance path stack.
	 * @param segment The path segment to push (e.g., "child" or "0"
	 */
	void push_instance_path_segment(const String &segment);

	/**
	 * @brief Pops the top segment from the instance path stack.
	 */
	void pop_instance_path_segment();

	/**
	 * @brief Pushes a resource path onto the resource context stack.
	 * @param resource_path The resource path to push (e.g., "res://path/to/resource")
	 */
	void push_resource_context(const String &resource_path);

	/**
	 * @brief Pops the top resource path from the resource context stack.
	 */
	void pop_resource_context();

	/**
	 * @brief Gets the current resource path context as a stack.
	 * @return Array The current resource path stack
	 */
	Array get_resource_path_stack() const;

	/**
	 * @brief Gets the underlying ryml parser instance.
	 *
	 * @return const ryml::Parser* Pointer to the ryml parser
	 */
	const ryml::Parser *get_ryml_parser() const;

	/**
	 * @brief Flag indicating whether style detection is enabled.
	 *
	 * When true, the parser will detect and preserve style information
	 * from the input YAML. When false, style detection is skipped.
	 */
	const bool detect_style;

private:
	/**
	 * @brief Pointer to the underlying ryml parser instance.
	 */
	const ryml::Parser *ryml_parser;

	/**
	 * @brief Stack of style objects for the current parsing context.
	 *
	 * This stack maintains the hierarchy of styles during parsing.
	 * The top of the stack is the current style being populated.
	 */
	std::stack<Ref<YAMLStyle>> style_stack;

	/**
	 * @brief Indicates if a root schema has been set for validation.
	 */
	bool has_root_schema = false;

	/**
	 * @brief Stack of schema nodes for validation context.
	 *
	 * This stack maintains the current position in the schema tree
	 * during validation. The top of the stack is the current schema.
	 */
	std::stack<Ref<Schema>> schema_stack;

	/**
	 * @brief List of validation errors encountered during parsing.
	 */
	std::vector<ValidationError> validation_errors;

	/**
	 * @brief Stack of path segments representing the current instance path.
	 */
	std::vector<String> instance_path_segments;

	/**
	 * @brief Stack of resource paths for nested resource contexts.
	 */
	std::vector<String> resource_path_stack;
};

} // namespace godot

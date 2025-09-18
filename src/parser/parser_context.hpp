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
	 */
	YAMLParserContext(const ryml::Parser *p_ryml_parser, const Ref<YAMLStyle> &root_style = nullptr);

	/**
	 * @brief Default destructor.
	 */
	~YAMLParserContext() = default;

	/**
	 * @brief Gets the current style from the top of the style stack.
	 *
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
};

} // namespace godot

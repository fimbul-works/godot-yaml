/**
 * @file exception.hpp
 * @brief Defines the YAMLException class for error handling in YAML operations.
 *
 * This file contains the YAMLException class which extends std::runtime_error
 * to provide YAML-specific error handling with location information and
 * integration with Godot's string system.
 */
#pragma once

#include "util/util_string.hpp"

#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <ryml.hpp>

#include <stdexcept>

namespace godot {

/**
 * @class YAMLException
 * @brief Exception class for YAML parsing and serialization errors.
 *
 * The YAMLException class extends std::runtime_error to provide enhanced
 * error reporting for YAML operations. It includes:
 * - Support for Godot String error messages
 * - Location information (line and column) for source document errors
 * - Integration with ryml's location tracking
 *
 * This allows for more detailed error messages that pinpoint the exact
 * location of YAML syntax or semantic errors.
 *
 * @extends std::runtime_error
 */
class YAMLException : public std::runtime_error {
public:
	/**
	 * @brief Constructs a YAMLException with a C-style message.
	 *
	 * @param msg The error message
	 * @param loc Optional location information
	 */
	explicit YAMLException(const char *msg, ryml::Location loc = {}) :
			std::runtime_error(msg),
			godot_message(msg),
			m_location(loc) {}

	/**
	 * @brief Constructs a YAMLException with a Godot String message.
	 *
	 * @param msg The error message
	 * @param loc Optional location information
	 */
	explicit YAMLException(String msg, ryml::Location loc = {}) :
			std::runtime_error(msg.utf8().get_data()),
			godot_message(std::move(msg)),
			m_location(loc) {}

	/**
	 * @brief Gets the error message as a Godot String.
	 *
	 * @return const String& The error message
	 */
	const String &get_godot_message() const { return godot_message; }

	/**
	 * @brief Gets the location information.
	 *
	 * @return const ryml::Location& The location in the source document
	 */
	const ryml::Location &get_location() const {
		return m_location;
	}

	/**
	 * @brief Gets the line number of the error.
	 *
	 * @return const int64_t The line number (0-based) or undefined if not available
	 */
	const int64_t get_line() const {
		return m_location.line;
	}

	/**
	 * @brief Gets the column number of the error.
	 *
	 * @return const int64_t The column number (0-based) or undefined if not available
	 */
	const int64_t get_column() const {
		return m_location.col;
	}

private:
	/**
	 * @brief The error message as a Godot String.
	 */
	String godot_message;

	/**
	 * @brief Location information for the error.
	 */
	ryml::Location m_location;
};

} // namespace godot

#ifndef YAML_EXCEPTION_H
#define YAML_EXCEPTION_H

#include "util_string.h"
#include <godot_cpp/variant/string.hpp>
#include <stdexcept>

namespace godot {

class YAMLException : public std::runtime_error {
public:
	// Constructor overloads that minimize conversions
	explicit YAMLException(const char *msg) :
			std::runtime_error(msg),
			godot_message(msg) {}

	explicit YAMLException(String msg) :
			std::runtime_error(msg.utf8().get_data()),
			godot_message(std::move(msg)) {}

	// Get the Godot String version without conversion
	const String &get_godot_message() const { return godot_message; }

	// Utility methods that avoid string conversions
	static YAMLException create_invalid_format(const char *type_name) {
		return YAMLException(String("Invalid ") + type_name + " format");
	}

	static YAMLException create_invalid_sequence_length(const char *type_name, int expected_length) {
		return YAMLException(String(type_name) + " sequence must have " + String::num_int64(expected_length) + " elements");
	}

	static YAMLException create_decode_error(const char *type_name, const char *details) {
		return YAMLException(String("Failed to decode ") + type_name + ": " + details);
	}

private:
	String godot_message; // Store Godot String to avoid reconversion
};

} // namespace godot

#endif // YAML_EXCEPTION_H

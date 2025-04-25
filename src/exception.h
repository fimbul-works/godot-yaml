#ifndef YAML_EXCEPTION_H
#define YAML_EXCEPTION_H

#include "util_string.h"

#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <stdexcept>

namespace godot {

class YAMLException : public std::runtime_error {
public:
	explicit YAMLException(const char *msg) :
			std::runtime_error(msg),
			godot_message(msg) {}

	explicit YAMLException(String msg) :
			std::runtime_error(msg.utf8().get_data()),
			godot_message(std::move(msg)) {}

	const String &get_godot_message() const { return godot_message; }

	static YAMLException create_invalid_format(const char *type_name) {
		return YAMLException(vformat("Invalid %s format", type_name));
	}

	static YAMLException create_invalid_sequence_length(const char *type_name, int expected_length) {
		return YAMLException(vformat("%s sequence must have %d elements", type_name, expected_length));
	}

	static YAMLException create_decode_error(const char *type_name, const char *details) {
		return YAMLException(vformat("Failed to decode %s: %s", type_name, details));
	}

private:
	String godot_message; // Store Godot String to avoid reconversion
};

} // namespace godot

#endif // YAML_EXCEPTION_H

#ifndef YAML_EXCEPTION_H
#define YAML_EXCEPTION_H

#include "util_string.h"

#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <ryml.hpp>
#include <stdexcept>

namespace godot {

class YAMLException : public std::runtime_error {
public:
	explicit YAMLException(const char *msg, ryml::Location loc = {}) :
			std::runtime_error(msg),
			godot_message(msg),
			m_location(loc) {}

	explicit YAMLException(String msg, ryml::Location loc = {}) :
			std::runtime_error(msg.utf8().get_data()),
			godot_message(std::move(msg)),
			m_location(loc) {}

	const String &get_godot_message() const { return godot_message; }

	const ryml::Location &get_location() const {
		return m_location;
	}

	const size_t get_line() const {
		return m_location.line;
	}

	const size_t get_column() const {
		return m_location.col;
	}

private:
	String godot_message;
	ryml::Location m_location;
};

} // namespace godot

#endif // YAML_EXCEPTION_H

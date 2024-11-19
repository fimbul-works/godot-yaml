#ifndef YAML_EXCEPTION_H
#define YAML_EXCEPTION_H

#include <godot_cpp/variant/string.hpp>
#include <stdexcept>

namespace godot {

class YAMLException : public std::runtime_error {
  public:
  // Constructor overloads
  explicit YAMLException(const String& msg) :
          std::runtime_error(msg.utf8().get_data()) { }

  explicit YAMLException(const std::string& what_arg) :
          std::runtime_error(what_arg) { }

  explicit YAMLException(const char* what_arg) :
          std::runtime_error(what_arg) { }

  // Utility methods for common error patterns
  static YAMLException create_invalid_format(const char* type_name)
  {
    return YAMLException(String("Invalid ") + type_name + " format");
  }

  static YAMLException create_missing_field(const char* type_name, const char* field_name)
  {
    return YAMLException(String(type_name) + " must have '" + field_name + "' field");
  }

  static YAMLException create_invalid_sequence_length(const char* type_name, int expected_length)
  {
    return YAMLException(String(type_name) + " sequence must have " + String::num_int64(expected_length) + " elements");
  }
};

} // namespace godot

#endif // YAML_EXCEPTION_H

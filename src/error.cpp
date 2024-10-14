#include "yaml.h"

#include <c4/format.hpp>

using namespace godot;

std::string YAML::error;

Variant YAML::get_error() const
{
  return error.empty() ? Variant() : String(error.c_str());
}

void YAML::error_callback(const char* message, size_t len, ryml::Location loc, void* user_data)
{
  ryml::csubstr error_msg(message, len);

  // Strip unnecessary error prefix
  const ryml::csubstr strip_error_prefix = "ERROR: ";
  if (error_msg.begins_with(strip_error_prefix)) {
    error_msg = error_msg.sub(strip_error_prefix.len);
  }

  // A more informative error message for complex keys
  if (error_msg.begins_with("ryml trees cannot handle containers as keys")) {
    error_msg = ryml::to_csubstr("unsupported complex key");
  }

  // Only return the first line
  size_t newline_pos = error_msg.find('\n');
  if (newline_pos != ryml::substr::npos) {
    error_msg = error_msg.sub(0, newline_pos);
  }

  // Compile the final error message
  std::string formatted_error;
  formatted_error.reserve(error_msg.len + 64); // Reserve extra to avoid reallocating
  ryml::formatrs(&formatted_error, "{} at line {}, column {}", error_msg, loc.line, loc.col);

  // Assign to the static error property
  error = std::move(formatted_error);

  throw YAMLException(error);
}

#ifndef UTIL_STRING_H
#define UTIL_STRING_H

#include <godot_cpp/variant/string.hpp>
#include <ryml.hpp>
#include <ryml_std.hpp>

namespace godot {

// Convert Godot String to ryml format
inline ryml::csubstr to_ryml_str(const String& str)
{
  if (str.is_empty()) {
    return ryml::csubstr {};
  }
  const CharString& utf8 = str.utf8();
  return ryml::csubstr(utf8.get_data(), utf8.length());
}

// Convert ryml string to Godot String
inline String from_ryml_str(const ryml::csubstr& str)
{
  if (str.len == ryml::csubstr::npos || str.empty()) {
    return String();
  }
  return String::utf8(str.str, str.len);
}

inline bool needs_block_style(const String& str)
{
  return str.contains("\n") || str.contains("\"") || str.begins_with(" ") || str.ends_with(" ") || str.begins_with("#");
}

inline bool needs_block_style(const ryml::csubstr& str)
{
  return needs_block_style(from_ryml_str(str));
}

inline bool should_use_block_style(const String& str)
{
  // Check for multiple lines
  if (str.find("\n") != -1) {
    return true;
  }

  // Check for leading/trailing spaces
  if (str.begins_with(" ") || str.ends_with(" ")) {
    return true;
  }

  // Check for special characters that might need escaping
  if (str.find(":") != -1 || str.find("#") != -1 || str.find("{") != -1 || str.find("}") != -1 || str.find("[") != -1 || str.find("]") != -1) {
    return true;
  }

  return false;
}

} // namespace godot

#endif // UTIL_STRING_H

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

// String format checks used in multiple converters
inline bool needs_block_style(const ryml::csubstr& str)
{
  // Convert only once for checking multiple conditions
  String godot_str = from_ryml_str(str);
  return godot_str.contains("\n") || godot_str.contains("\"") || godot_str.begins_with(" ") || godot_str.ends_with(" ") || godot_str.begins_with("#");
}

inline bool is_multiline(const ryml::csubstr& str)
{
  return str.first('\n') != ryml::csubstr::npos;
}

} // namespace godot

#endif // UTIL_STRING_H

#ifndef UTIL_NUMERIC_H
#define UTIL_NUMERIC_H

#include "yaml.h"
#include "yaml_exception.h"

#include <c4/format.hpp>
#include <ryml.hpp>

#include <charconv>
#include <limits>
#include <type_traits>

namespace godot {

template <typename T>
T string_to_float(const ryml::csubstr& value)
{
  static_assert(std::is_floating_point<T>::value, "Type must be floating point");

  if (value == ".nan") {
    return std::numeric_limits<T>::quiet_NaN();
  } else if (value == ".inf" || value == "+.inf") {
    return std::numeric_limits<T>::infinity();
  } else if (value == "-.inf") {
    return -std::numeric_limits<T>::infinity();
  } else {
    auto span = value.first_real_span();
    if (span.empty()) {
      throw YAMLException("Invalid float format");
    }

    T result;
    auto [ptr, ec] = std::from_chars(span.begin(), span.end(), result);

    if (ec == std::errc()) {
      return result;
    } else if (ec == std::errc::result_out_of_range) {
      throw YAMLException("Float value out of range");
    } else {
      throw YAMLException("Unknown error in float conversion");
    }
  }
}

template <typename T>
T string_to_int(const ryml::csubstr& value)
{
  static_assert(std::is_integral<T>::value, "Type must be integral");

  T result;
  int base = 10;
  const char* start = value.begin();
  const char* end = value.end();

  // Handle sign
  bool is_negative = false;
  if (value.begins_with('-')) {
    is_negative = true;
    start++;
  } else if (value.begins_with('+')) {
    start++;
  }

  // Determine base and starting point
  if (end - start >= 2 && start[0] == '0') {
    if (start[1] == 'x' || start[1] == 'X') {
      base = 16;
      start += 2;
    } else if (start[1] == 'b' || start[1] == 'B') {
      base = 2;
      start += 2;
    } else if (start[1] == 'o' || start[1] == 'O') {
      base = 8;
      start += 2;
    } else {
      base = 8; // Octal with leading 0
    }
  }

  auto [ptr, ec] = std::from_chars(start, end, result, base);

  if (ec == std::errc()) {
    return is_negative ? -result : result;
  } else if (ec == std::errc::result_out_of_range) {
    throw YAMLException("Integer value out of range");
  } else {
    throw YAMLException("Invalid integer format");
  }
}

template <typename T>
ryml::csubstr float_to_string(const T value)
{
  static_assert(std::is_floating_point<T>::value, "Type must be floating point");

  if (std::isnan(value)) {
    return ".nan";
  } else if (std::isinf(value)) {
    return value > 0 ? ".inf" : "-.inf";
  } else {
    static thread_local char buf[64];
    size_t len = ryml::format(buf, "{}", value);
    return ryml::csubstr(buf, len);
  }
}

template <typename T>
ryml::csubstr int_to_string(const T value, const int base = 10)
{
  static_assert(std::is_integral<T>::value, "Type must be integral");

  static thread_local char buf[64];
  const char* format;

  switch (base) {
    case 2:
      format = "0b{:b}";
      break;
    case 8:
      format = "0o{:o}";
      break;
    case 16:
      format = "0x{:x}";
      break;
    default:
      format = "{}";
      break;
  }

  size_t len = ryml::format(buf, format, value);
  return ryml::csubstr(buf, len);
}

// Overloads for different input types
template <typename T>
T string_to_float(const ryml::substr& value)
{
  return string_to_float<T>(ryml::to_csubstr(value));
}

template <typename T>
T string_to_float(const std::string& value)
{
  return string_to_float<T>(ryml::to_csubstr(value));
}

template <typename T>
T string_to_float(const godot::String& value)
{
  return string_to_float<T>(ryml::to_csubstr(value.utf8().get_data()));
}

template <typename T>
T string_to_int(const ryml::substr& value)
{
  return string_to_int<T>(ryml::to_csubstr(value));
}

template <typename T>
T string_to_int(const std::string& value)
{
  return string_to_int<T>(ryml::to_csubstr(value));
}

template <typename T>
T string_to_int(const godot::String& value)
{
  return string_to_int<T>(ryml::to_csubstr(value.utf8().get_data()));
}

} // namespace godot

#endif // UTIL_NUMERIC_H

#ifndef UTIL_NUMERIC_H
#define UTIL_NUMERIC_H

#include "exception.h"
#include "yaml.h"

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
ryml::csubstr float_to_string(const T value, YAMLStyle::NumberFormat format = YAMLStyle::NUM_DECIMAL)
{
  static_assert(std::is_floating_point<T>::value, "Type must be floating point");

  if (std::isnan(value)) {
    return ".nan";
  } else if (std::isinf(value)) {
    return value > 0 ? ".inf" : "-.inf";
  } else {
    static thread_local char buf[64];
    const char* format_str;

    switch (format) {
      case YAMLStyle::NUM_SCIENTIFIC: {
        // Manual scientific notation formatting
        int exp = 0;
        T mantissa = value;

        if (mantissa != 0) {
          while (std::abs(mantissa) >= 10.0) {
            mantissa /= 10.0;
            exp++;
          }
          while (std::abs(mantissa) < 1.0) {
            mantissa *= 10.0;
            exp--;
          }
        }

        size_t len = snprintf(buf, sizeof(buf), "%.6fe%+d", mantissa, exp);
        return ryml::csubstr(buf, len);
      }
      case YAMLStyle::NUM_HEX:
        format_str = "0x{:a}"; // Hexadecimal floating point format
        break;
      default:
        format_str = "{}";
        break;
    }

    size_t len = ryml::format(buf, format_str, value);
    return ryml::csubstr(buf, len);
  }
}

template <typename T>
ryml::csubstr int_to_string(const T value, YAMLStyle::NumberFormat format = YAMLStyle::NUM_DECIMAL)
{
  static_assert(std::is_integral<T>::value, "Type must be integral");
  static thread_local char buf[64];

  switch (format) {
    case YAMLStyle::NUM_HEX:
      snprintf(buf, sizeof(buf), "0x%x", (unsigned int)value);
      break;
    case YAMLStyle::NUM_OCTAL:
      snprintf(buf, sizeof(buf), "0o%o", (unsigned int)value);
      break;
    case YAMLStyle::NUM_BINARY: {
      // Handle binary format manually since snprintf doesn't support it
      char binary[65]; // max 64 bits plus null terminator
      T temp = value;
      int i = 0;
      do {
        binary[i++] = '0' + (temp & 1);
        temp >>= 1;
      } while (temp && i < 64);

      // Reverse and add prefix
      snprintf(buf, sizeof(buf), "0b");
      for (int j = i - 1; j >= 0; j--) {
        size_t len = strlen(buf);
        buf[len] = binary[j];
        buf[len + 1] = '\0';
      }
      break;
    }
    case YAMLStyle::NUM_SCIENTIFIC:
      snprintf(buf, sizeof(buf), "%e", (double)value);
      break;
    default:
      snprintf(buf, sizeof(buf), "%d", (int)value);
      break;
  }

  return ryml::csubstr(buf, strlen(buf));
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

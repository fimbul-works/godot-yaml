/**
 * @file util_numeric.hpp
 * @brief Utility functions for numeric type conversion between YAML and Godot.
 *
 * This file provides template functions for converting between string representations
 * in YAML and native numeric types in Godot, with support for different numeric
 * formats such as decimal, hexadecimal, octal, and binary.
 */
#pragma once

#include "exception.hpp"
#include "style.hpp"

#include <c4/format.hpp>
#include <ryml.hpp>

#include <charconv>
#include <limits>
#include <type_traits>

namespace godot {

/**
 * @brief Converts a string to an integer type with format detection.
 *
 * This template function converts a string representation in various formats
 * (decimal, hexadecimal, octal, binary) to an integer type, with optional
 * detection of the format used.
 *
 * @tparam T The integer type to convert to (must be an integral type)
 * @param value The string value to convert
 * @param format Optional pointer to store the detected format
 * @return T The converted integer value
 * @throws std::exception If the value is invalid or out of range
 */
template <typename T>
T string_to_int(const ryml::csubstr &value, YAMLStyle::IntegerFormat *format = nullptr) {
	static_assert(std::is_integral<T>::value, "Type must be integral");

	T result;
	int base = 10;
	const char *start = value.begin();
	const char *end = value.end();

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
			if (format != nullptr) {
				*format = YAMLStyle::INT_HEX;
			}
		} else if (start[1] == 'b' || start[1] == 'B') {
			base = 2;
			start += 2;
			if (format != nullptr) {
				*format = YAMLStyle::INT_BINARY;
			}
		} else if (start[1] == 'o' || start[1] == 'O') {
			base = 8;
			start += 2;
			if (format != nullptr) {
				*format = YAMLStyle::INT_OCTAL;
			}
		} else {
			base = 8; // Octal with leading 0
			if (format != nullptr) {
				*format = YAMLStyle::INT_OCTAL;
			}
		}
	} else if (format != nullptr) {
		*format = YAMLStyle::INT_DECIMAL;
	}

	auto [ptr, ec] = std::from_chars(start, end, result, base);

	if (ec == std::errc()) {
		return is_negative ? -result : result;
	} else if (ec == std::errc::result_out_of_range) {
		throw YAMLException("Integer value out of range");
	}

	throw YAMLException("Invalid integer format");
}

/**
 * @brief Overload of string_to_int for ryml::substr.
 *
 * @tparam T The integer type to convert to
 * @param value The string value to convert
 * @param format Optional pointer to store the detected format
 * @return T The converted integer value
 */
template <typename T>
T string_to_int(const ryml::substr &value, YAMLStyle::IntegerFormat *format = nullptr) {
	return string_to_int<T>(ryml::to_csubstr(value), format);
}

/**
 * @brief Overload of string_to_int for Godot String.
 *
 * @tparam T The integer type to convert to
 * @param value The string value to convert
 * @param format Optional pointer to store the detected format
 * @return T The converted integer value
 */
template <typename T>
T string_to_int(const godot::String &value, YAMLStyle::IntegerFormat *format = nullptr) {
	return string_to_int<T>(ryml::to_csubstr(value.utf8().get_data()), format);
}

/**
 * @brief Converts an integer to a string with a specified format.
 *
 * @tparam T The integer type to convert
 * @param value The integer value to convert
 * @param format The format to use for the output string
 * @return ryml::csubstr A view of the converted string
 */
template <typename T>
ryml::csubstr int_to_string(const T value, YAMLStyle::IntegerFormat format = YAMLStyle::INT_DECIMAL) {
	static_assert(std::is_integral<T>::value, "Type must be integral");
	static thread_local char buf[64];

	switch (format) {
		case YAMLStyle::INT_HEX:
			snprintf(buf, sizeof(buf), "0x%llx", (uint64_t)value);
			break;
		case YAMLStyle::INT_OCTAL:
			snprintf(buf, sizeof(buf), "0o%llo", (uint64_t)value);
			break;
		case YAMLStyle::INT_BINARY: {
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
		default:
			snprintf(buf, sizeof(buf), "%lld", (int64_t)value);
			break;
	}

	return ryml::csubstr(buf, strlen(buf));
}

/**
 * @brief Converts a string to a floating-point type with format detection.
 *
 * Handles special YAML values like .nan, .inf, and -.inf.
 *
 * @tparam T The floating-point type to convert to
 * @param value The string value to convert
 * @param format Optional pointer to store the detected format
 * @return T The converted floating-point value
 * @throws std::exception If the value is invalid or out of range
 */
template <typename T>
T string_to_float(const ryml::csubstr &value, YAMLStyle::FloatFormat *format = nullptr) {
	static_assert(std::is_floating_point<T>::value, "Type must be floating point");

	if (value == ".nan") {
		return std::numeric_limits<T>::quiet_NaN();
	} else if (value == ".inf" || value == "+.inf") {
		return std::numeric_limits<T>::infinity();
	} else if (value == "-.inf") {
		return -std::numeric_limits<T>::infinity();
	}

	auto span = value.first_real_span();
	if (span.empty()) {
		throw YAMLException("Invalid float format");
	}

	T result;
	// Use standard library functions for floating-point parsing
	// (works on all platforms, including Apple where std::from_chars for floats is missing)
	std::string str(span.begin(), span.end());
	char *end_ptr;

	if constexpr (std::is_same_v<T, float>) {
		result = std::strtof(str.c_str(), &end_ptr);
	} else if constexpr (std::is_same_v<T, double>) {
		result = std::strtod(str.c_str(), &end_ptr);
	} else {
		result = std::strtold(str.c_str(), &end_ptr);
	}

	// Check if conversion was successful
	if (end_ptr == str.c_str()) {
		throw YAMLException("Invalid float format");
	}

	// Check for out of range (infinity values)
	if (result == std::numeric_limits<T>::infinity() || result == -std::numeric_limits<T>::infinity()) {
		if (str != ".inf" && str != "-.inf" && str != "+.inf") {
			throw YAMLException("Float value out of range");
		}
	}

	if (format != nullptr) {
		if (value.find("e") != ryml::npos || value.find("E") != ryml::npos) {
			*format = YAMLStyle::FLOAT_SCIENTIFIC;
		} else {
			*format = YAMLStyle::FLOAT_DECIMAL;
		}
	}

	throw YAMLException("Invalid float format");
}

/**
 * @brief Overload of string_to_float for ryml::substr.
 *
 * @tparam T The floating-point type to convert to
 * @param value The string value to convert
 * @param format Optional pointer to store the detected format
 * @return T The converted floating-point value
 */
template <typename T>
T string_to_float(const ryml::substr &value, YAMLStyle::FloatFormat *format = nullptr) {
	return string_to_float<T>(ryml::to_csubstr(value), format);
}

/**
 * @brief Overload of string_to_float for Godot String.
 *
 * @tparam T The floating-point type to convert to
 * @param value The string value to convert
 * @param format Optional pointer to store the detected format
 * @return T The converted floating-point value
 */
template <typename T>
T string_to_float(const godot::String &value, YAMLStyle::FloatFormat *format = nullptr) {
	return string_to_float<T>(ryml::to_csubstr(value.utf8().get_data()), format);
}

/**
 * @brief Converts a floating-point value to a string with a specified format.
 *
 * Handles special values like NaN and infinity according to YAML conventions.
 *
 * @tparam T The floating-point type to convert
 * @param value The floating-point value to convert
 * @param format The format to use for the output string
 * @return ryml::csubstr A view of the converted string
 */
template <typename T>
ryml::csubstr float_to_string(const T value, YAMLStyle::FloatFormat format = YAMLStyle::FLOAT_DECIMAL) {
	static_assert(std::is_floating_point<T>::value, "Type must be floating point");

	if (std::isnan(value)) {
		return ".nan";
	} else if (std::isinf(value)) {
		return value > 0 ? ".inf" : "-.inf";
	}

	static thread_local char buf[64];
	const char *format_str;

	switch (format) {
		case YAMLStyle::FLOAT_SCIENTIFIC: {
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
		default:
			format_str = "{}";
			break;
	}

	size_t len = ryml::format(buf, format_str, value);
	return ryml::csubstr(buf, len);
}

} // namespace godot

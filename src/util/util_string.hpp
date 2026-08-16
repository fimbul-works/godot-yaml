/**
 * @file util_string.hpp
 * @brief Utility functions for string operations in YAML processing.
 *
 * This file provides helper functions for converting between Godot strings
 * and ryml strings, as well as functions to determine the appropriate
 * YAML style for string values.
 */
#pragma once

#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <regex>
#include <ryml.hpp>
#include <ryml_std.hpp>

namespace godot {

/**
 * @brief Converts a ryml string to a Godot String.
 *
 * @param str The ryml substring to convert
 * @return String A Godot String created from the UTF-8 data
 */
inline String from_ryml_str(const ryml::csubstr &str) {
	if (str.len == ryml::csubstr::npos || str.empty()) {
		return String();
	}
	return String::utf8(str.str, str.len);
}

/**
 * @brief Determines if a string needs block style in YAML.
 *
 * Block style is needed for strings containing newlines, double quotes,
 * or beginning/ending with spaces or special characters.
 *
 * @param str The string to check
 * @return bool True if the string should use block style
 */
inline bool needs_block_style(const String &str) {
	return !str.strip_edges().is_empty() && (str.contains("\n") || str.contains("\"") || str.begins_with(" ") || str.ends_with(" ") || str.begins_with("#"));
}

/**
 * @brief Overload of needs_block_style for ryml::csubstr.
 *
 * @param str The ryml substring to check
 * @return bool True if the string should use block style
 */
inline bool needs_block_style(const ryml::csubstr &str) {
	return needs_block_style(from_ryml_str(str));
}

/**
 * @brief Determines if a string needs quotes in YAML.
 *
 * Quotes are needed for empty strings, strings beginning/ending with spaces,
 * beginning with '#', containing special characters like colons
 * or brackets that might be interpreted as YAML syntax.
 *
 * @param value The string to check
 * @return bool True if the string should be quoted
 */
inline bool needs_quotes(const String &value) {
	if (value.is_empty()) {
		return true;
	}

	if (value.begins_with(" ") || value.ends_with(" ") || value.begins_with("#")) {
		return true;
	}

	String lower = value.to_lower();

	// YAML reserved plain scalars
	if (lower == "null" || lower == "~" ||
			lower == "true" || lower == "false" ||
			lower == "y" || lower == "n" ||
			lower == "yes" || lower == "no" ||
			lower == ".inf" || lower == "-.inf" || lower == "+.inf" || lower == ".nan") {
		return true;
	}

	// YAML special syntax characters
	const char32_t *chars = value.ptr();
	int64_t len = value.length();
	for (int64_t i = 0; i < len; i++) {
		char32_t c = chars[i];
		switch (c) {
			case '{':
			case '}':
			case '[':
			case ']':
			case ',':
			case '&':
			case ':':
			case '*':
			case '?':
			case '|':
			case '-':
			case '<':
			case '>':
			case '=':
			case '!':
			case '%':
			case '@':
			case '/':
			case '\\':
				return true;
			default:
				break;
		}
	}

	// Looks numeric? Then quote it.
	// Matches integers, floats, scientific, hex, octal, binary, or leading zeros.
	static const std::regex numeric_re("^(?:[-+]?\\d+(?:\\.\\d+)?(?:e[-+]?\\d+)?|0x[0-9a-f]+|0o[0-7]+|0b[01]+|0\\d+)$");
	CharString utf8 = lower.utf8();
	if (std::regex_match(utf8.get_data(), numeric_re)) {
		return true;
	}

	return false;
}

/**
 * @brief Overload of needs_quotes for ryml::csubstr.
 *
 * @param str The ryml substring to check
 * @return bool True if the string should be quoted
 */
inline bool needs_quotes(const ryml::csubstr &str) {
	return needs_quotes(from_ryml_str(str));
}

/**
 * @brief Get godot::Variant type name.
 * @param value Variant value
 * @return Variant type name
 */
inline String type_str(const Variant &value) {
	return Variant::get_type_name(value.get_type());
}

} // namespace godot

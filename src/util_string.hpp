/**
 * @file util_string.hpp
 * @brief Utility functions for string operations in YAML processing.
 *
 * This file provides helper functions for converting between Godot strings
 * and ryml strings, as well as functions to determine the appropriate
 * YAML style for string values.
 */
#pragma once

#include <godot_cpp/classes/reg_ex.hpp>
#include <godot_cpp/classes/reg_ex_match.hpp>
#include <godot_cpp/variant/string.hpp>
#include <ryml.hpp>
#include <ryml_std.hpp>

namespace godot {

/**
 * @brief Converts a Godot String to ryml string format.
 *
 * @param str The Godot String to convert
 * @return ryml::csubstr A constant substring view of the UTF-8 string data
 */
inline ryml::csubstr to_ryml_str(const String &str) {
	if (str.is_empty()) {
		return ryml::csubstr{};
	}
	const CharString &utf8 = str.utf8();
	return ryml::csubstr(utf8.get_data(), utf8.length());
}

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
	return str.contains("\n") || str.contains("\"") || str.begins_with(" ") || str.ends_with(" ") || str.begins_with("#");
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
 * Quotes are needed for strings beginning/ending with spaces,
 * beginning with '#', or containing special characters like colons
 * or brackets that might be interpreted as YAML syntax.
 *
 * @param value The string to check
 * @return bool True if the string should be quoted
 */
inline bool needs_quotes(const String &value) {
	if (value.begins_with(" ") || value.ends_with(" ") || value.begins_with("#")) {
		return true;
	}
	Ref<RegEx> special_reg_ex = RegEx::create_from_string("[{}\\[\\],&:\\*?|\\-<>=!%@\\/]");
	return special_reg_ex->search(value).is_valid();
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

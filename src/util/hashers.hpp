/**
 * @file hashers.hpp
 * @brief Provides hash and equality functors for Godot string types.
 *
 * This file defines utility structs for hashing and comparing Godot's String
 * and StringName types, enabling their use as keys in standard C++ containers
 * like unordered_map and unordered_set.
 */
#pragma once

#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/variant/variant.hpp>

#include <unordered_map>

namespace godot {

/**
 * @struct StringHasher
 * @brief Functor for hashing Godot String objects.
 *
 * Enables String to be used as a key in unordered containers by
 * providing a hash function using String's internal hash method.
 */
struct StringHasher {
	/**
	 * @brief Calculates a hash value for a Godot String.
	 *
	 * @param str The String to hash
	 * @return int64_t The hash value
	 */
	int64_t operator()(const String &str) const { return str.hash(); }
};

/**
 * @struct StringNameHasher
 * @brief Functor for hashing Godot StringName objects.
 *
 * Enables StringName to be used as a key in unordered containers by
 * providing a hash function using StringName's internal hash method.
 */
struct StringNameHasher {
	/**
	 * @brief Calculates a hash value for a Godot StringName.
	 *
	 * @param str The StringName to hash
	 * @return int64_t The hash value
	 */
	int64_t operator()(const StringName &str) const { return str.hash(); }
};

/**
 * @struct StringEqual
 * @brief Functor for comparing Godot String objects for equality.
 *
 * Provides equality comparison for String objects when used as keys
 * in unordered containers.
 */
struct StringEqual {
	/**
	 * @brief Checks if two String objects are equal.
	 *
	 * @param lhs The left-hand side String
	 * @param rhs The right-hand side String
	 * @return bool True if the strings are equal
	 */
	bool operator()(const String &lhs, const String &rhs) const { return lhs == rhs; }
};

/**
 * @struct StringNameEqual
 * @brief Functor for comparing Godot StringName objects for equality.
 *
 * Provides equality comparison for StringName objects when used as keys
 * in unordered containers.
 */
struct StringNameEqual {
	/**
	 * @brief Checks if two StringName objects are equal.
	 *
	 * @param lhs The left-hand side StringName
	 * @param rhs The right-hand side StringName
	 * @return bool True if the StringNames are equal
	 */
	bool operator()(const StringName &lhs, const StringName &rhs) const { return lhs == rhs; }
};

} // namespace godot

/**
 * @brief STL namespace specialization for hashing Godot String.
 *
 * This specialization allows Godot String to be used directly with STL
 * containers without custom hash functors.
 */
namespace std {

template <> struct hash<godot::String> {
	/**
	 * @brief Calculates a hash value for a Godot String.
	 *
	 * @param str The String to hash
	 * @return int64_t The hash value
	 */
	int64_t operator()(const godot::String &str) const { return str.hash(); }
};

template <> struct hash<godot::Variant> {
	/**
	 * @brief Calculates a hash value for a Godot Variant.
	 *
	 * @param str The Variant to hash
	 * @return int64_t The hash value
	 */
	int64_t operator()(const godot::Variant &var) const { return var.hash(); }
};

} //namespace std

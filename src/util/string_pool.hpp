/**
 * @file string_pool.hpp
 * @brief Defines a string pooling mechanism for efficient memory use.
 *
 * This file contains the YAMLStringPool class which provides memory-efficient
 * string storage during YAML operations. It prevents duplicate strings by
 * maintaining a pool of unique string instances and returning views to them.
 */
#pragma once

#include <godot_cpp/variant/string.hpp>
#include <ryml.hpp>

#include <unordered_set>

namespace godot {

/**
 * @class YAMLStringPool
 * @brief Provides memory-efficient string storage during YAML operations.
 *
 * The YAMLStringPool class stores unique string data and provides
 * non-owning views (csubstr) to the stored data. This is particularly
 * useful during YAML parsing and emitting operations to avoid redundant
 * string allocations and improve performance.
 *
 * It maintains an internal collection of unique CharString instances
 * and ensures they remain alive while views to them are in use.
 */
class YAMLStringPool {
public:
	/**
	 * @brief Default constructor.
	 */
	YAMLStringPool() = default;

	/**
	 * @brief Non-copyable to prevent unintentional duplication.
	 */
	YAMLStringPool(const YAMLStringPool &) = delete;
	YAMLStringPool &operator=(const YAMLStringPool &) = delete;

	/**
	 * @brief Stores a Godot String and returns a view to its data.
	 *
	 * Converts the String to UTF-8, stores it in the pool if not already
	 * present, and returns a view to the stored data.
	 *
	 * @param str The String to store
	 * @return ryml::csubstr A view to the stored string data
	 */
	ryml::csubstr store(const String &str) {
		if (str.is_empty()) {
			return ryml::csubstr{};
		}

		// Create and store the CharString
		auto [it, inserted] = char_storage.insert(str.utf8());

		// Return a view of the stored CharString
		return ryml::csubstr(it->get_data(), it->length());
	}

	/**
	 * @brief Stores a C-style string directly and returns a view.
	 *
	 * @param cstr The C-style string to store
	 * @param len Length of the string
	 * @return ryml::csubstr A view to the stored string data
	 */
	ryml::csubstr store(const char *cstr, size_t len) {
		if (!cstr || len == 0) {
			return ryml::csubstr{};
		}

		// Create temporary CharString from the data
		CharString temp;
		temp.resize(len + 1);
		memcpy(temp.ptrw(), cstr, len);
		temp[len] = '\0';

		// Store and get reference
		auto [it, inserted] = char_storage.insert(std::move(temp));

		// Return a view of the stored data
		return ryml::csubstr(it->get_data(), it->length());
	}

	/**
	 * @brief Gets the number of unique strings in the pool.
	 *
	 * @return size_t Number of unique strings
	 */
	size_t size() const {
		return char_storage.size();
	}

private:
	/**
	 * @struct CharStringHasher
	 * @brief Hash functor for CharString.
	 */
	struct CharStringHasher {
		/**
		 * @brief Computes a hash for a CharString.
		 *
		 * @param str The CharString to hash
		 * @return int64_t The hash value
		 */
		int64_t operator()(const CharString &str) const {
			// Reuse String's hash function for CharString's content
			return String(str).hash();
		}
	};

	/**
	 * @struct CharStringEqual
	 * @brief Equality comparison functor for CharString.
	 */
	struct CharStringEqual {
		/**
		 * @brief Checks if two CharStrings are equal.
		 *
		 * @param a First CharString
		 * @param b Second CharString
		 * @return bool True if the strings are equal
		 */
		bool operator()(const CharString &a, const CharString &b) const {
			return a.length() == b.length() && memcmp(a.get_data(), b.get_data(), a.length()) == 0;
		}
	};

	/**
	 * @brief Storage for unique CharStrings.
	 *
	 * Uses a custom hash set to store unique CharString instances.
	 */
	std::unordered_set<CharString, CharStringHasher, CharStringEqual> char_storage;
};

} // namespace godot

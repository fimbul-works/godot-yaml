// string_pool.h
#ifndef YAML_STRING_POOL_H
#define YAML_STRING_POOL_H

#include <godot_cpp/variant/string.hpp>
#include <ryml.hpp>
#include <unordered_set>

namespace godot {

class YAMLStringPool {
public:
	// Default constructor
	YAMLStringPool() = default;

	// Prevent copying - pools shouldn't be copied
	YAMLStringPool(const YAMLStringPool &) = delete;
	YAMLStringPool &operator=(const YAMLStringPool &) = delete;

	// Store a String and get a ryml::csubstr view to its data
	ryml::csubstr store(const String &str) {
		if (str.is_empty()) {
			return ryml::csubstr{};
		}

		// Create and store the CharString
		auto [it, inserted] = char_storage.insert(str.utf8());

		// Return a view of the stored CharString
		return ryml::csubstr(it->get_data(), it->length());
	}

	// Store a char* directly (for C-style strings)
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

	// Get number of unique strings in the pool
	size_t size() const {
		return char_storage.size();
	}

private:
	// Hash and equality functions for CharString
	struct CharStringHasher {
		size_t operator()(const CharString &str) const {
			// Reuse String's hash function for CharString's content
			return String(str).hash();
		}
	};

	struct CharStringEqual {
		bool operator()(const CharString &a, const CharString &b) const {
			return a.length() == b.length() && memcmp(a.get_data(), b.get_data(), a.length()) == 0;
		}
	};

	// Storage for unique CharStrings
	std::unordered_set<CharString, CharStringHasher, CharStringEqual> char_storage;
};

} // namespace godot

#endif // YAML_STRING_POOL_H

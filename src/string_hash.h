#ifndef STRING_HASH_H
#define STRING_HASH_H

#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/string_name.hpp>

namespace godot {

struct StringHasher {
	size_t operator()(const String &str) const {
		return str.hash();
	}
};

struct StringNameHasher {
	size_t operator()(const StringName &str) const {
		return str.hash();
	}
};

struct StringEqual {
	bool operator()(const String &lhs, const String &rhs) const {
		return lhs == rhs;
	}
};

struct StringNameEqual {
	bool operator()(const StringName &lhs, const StringName &rhs) const {
		return lhs == rhs;
	}
};

} // namespace godot

namespace std {

template <>
struct hash<godot::String> {
	size_t operator()(const godot::String &s) const {
		return s.hash();
	}
};

} //namespace std

#endif // STRING_HASH_H

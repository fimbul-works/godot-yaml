#ifndef STRING_HASH_H
#define STRING_HASH_H

#include <godot_cpp/variant/string.hpp>

namespace std {
template <>
struct hash<godot::String> {
  size_t operator()(const godot::String& str) const
  {
    // Use Godot's built-in hash function
    return str.hash();
  }
};
}

#endif // STRING_HASH_H

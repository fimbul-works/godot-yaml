// string_hash.h
#ifndef STRING_HASH_H
#define STRING_HASH_H

#include <godot_cpp/variant/string.hpp>

namespace godot {

struct StringHasher {
  size_t operator()(const String& str) const
  {
    return str.hash();
  }
};

struct StringEqual {
  bool operator()(const String& lhs, const String& rhs) const
  {
    return lhs == rhs;
  }
};

} // namespace godot

#endif // STRING_HASH_H

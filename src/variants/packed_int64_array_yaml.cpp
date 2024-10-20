#include "packed_int64_array_yaml.h"
#include "util_numeric.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

const char* PackedInt64ArrayYAMLEncoder::get_tag() const
{
  return "PackedInt64Array";
}

void PackedInt64ArrayYAMLEncoder::encode(ryml::NodeRef& node, const Variant& v) const
{
  PackedInt64Array array = v.operator PackedInt64Array();
  node |= ryml::SEQ;
  for (const auto& f : array) {
    node.append_child() << int_to_string(f);
  }
}

Variant PackedInt64ArrayYAMLEncoder::decode(const ryml::ConstNodeRef& node) const
{
  if (node.is_seq()) {
    PackedInt64Array array = PackedInt64Array();
    int size = node.num_children();
    array.resize(size);
    for (int i = 0; i < size; ++i) {
      array[i] = string_to_int<int64_t>(node.child(i).val());
    }
    return array;
  }
  throw YAMLException("invalid PackedInt64Array format - " + String::utf8(node.val().str, node.val().len));
}

bool PackedInt64ArrayYAMLEncoder::set_format(const String& format_str)
{
  return true;
}

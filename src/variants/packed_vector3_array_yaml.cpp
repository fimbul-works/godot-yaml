#include "packed_vector3_array_yaml.h"
#include "yaml.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

const char* PackedVector3ArrayYAMLEncoder::get_tag() const
{
  return "PackedVector3Array";
}

void PackedVector3ArrayYAMLEncoder::encode(ryml::NodeRef& node, const Variant& v) const
{
  PackedVector3Array array = v.operator PackedVector3Array();
  node |= ryml::SEQ;
  for (const auto& color : array) {
    ryml::NodeRef color_node = node.append_child();
    vec_encoder.encode(color_node, color);
  }
}

Variant PackedVector3ArrayYAMLEncoder::decode(const ryml::ConstNodeRef& node) const
{
  if (node.is_seq()) {
    PackedVector3Array array = PackedVector3Array();
    int size = node.num_children();
    array.resize(size);
    for (int i = 0; i < size; ++i) {
      array[i] = vec_encoder.decode(node.child(i));
    }
    return array;
  }
  throw YAMLException("invalid PackedVector3Array format - " + String::utf8(node.val().str, node.val().len));
}

bool PackedVector3ArrayYAMLEncoder::set_format(const String& format_str)
{
  return vec_encoder.set_format(format_str);
}

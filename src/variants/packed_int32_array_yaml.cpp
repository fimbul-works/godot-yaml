#include "packed_int32_array_yaml.h"
#include "util_numeric.h"
#include "yaml.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

PackedInt32ArrayVariantConverter::PackedInt32ArrayVariantConverter(YAML* yaml) :
        VariantConverter(yaml) { }

void PackedInt32ArrayVariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  PackedInt32Array array = v.operator PackedInt32Array();
  node |= ryml::SEQ;
  for (const auto& i : array) {
    node.append_child() << int_to_string(i);
  }
}

Variant PackedInt32ArrayVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (node.is_seq()) {
    PackedInt32Array array = PackedInt32Array();
    int size = node.num_children();
    array.resize(size);
    for (int i = 0; i < size; ++i) {
      array[i] = string_to_int<int32_t>(node.child(i).val());
    }
    return array;
  }
  throw YAMLException("invalid PackedInt32Array format - " + String::utf8(node.val().str, node.val().len));
}

bool PackedInt32ArrayVariantConverter::set_format(const String& format_str)
{
  return true;
}

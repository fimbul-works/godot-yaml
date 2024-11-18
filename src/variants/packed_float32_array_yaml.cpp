#include "packed_float32_array_yaml.h"
#include "util_numeric.h"
#include "yaml.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

PackedFloat32ArrayVariantConverter::PackedFloat32ArrayVariantConverter(YAML* yaml) :
        VariantConverter(yaml) { }

void PackedFloat32ArrayVariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  PackedFloat32Array array = v.operator PackedFloat32Array();
  node |= ryml::SEQ;
  for (const auto& f : array) {
    node.append_child() << float_to_string(f);
  }
}

Variant PackedFloat32ArrayVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (node.is_seq()) {
    PackedFloat32Array array = PackedFloat32Array();
    int size = node.num_children();
    array.resize(size);
    for (int i = 0; i < size; ++i) {
      array[i] = string_to_float<float>(node.child(i).val());
    }
    return array;
  }
  throw YAMLException("invalid PackedFloat32Array format - " + String::utf8(node.val().str, node.val().len));
}

bool PackedFloat32ArrayVariantConverter::set_format(const String& format_str)
{
  return true;
}

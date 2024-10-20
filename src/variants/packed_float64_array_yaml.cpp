#include "packed_float64_array_yaml.h"
#include "util_numeric.h"
#include "yaml.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

PackedFloat64ArrayYAMLEncoder::PackedFloat64ArrayYAMLEncoder(YAML* yaml) :
        YAMLEncoder(yaml) { }

void PackedFloat64ArrayYAMLEncoder::encode(ryml::NodeRef& node, const Variant& v) const
{
  PackedFloat64Array array = v.operator PackedFloat64Array();
  node |= ryml::SEQ;
  for (const auto& f : array) {
    node.append_child() << float_to_string(f);
  }
}

Variant PackedFloat64ArrayYAMLEncoder::decode(const ryml::ConstNodeRef& node) const
{
  if (node.is_seq()) {
    PackedFloat64Array array = PackedFloat64Array();
    int size = node.num_children();
    array.resize(size);
    for (int i = 0; i < size; ++i) {
      array[i] = string_to_float<double>(node.child(i).val());
    }
    return array;
  }
  throw YAMLException("invalid PackedFloat64Array format - " + String::utf8(node.val().str, node.val().len));
}

bool PackedFloat64ArrayYAMLEncoder::set_format(const String& format_str)
{
  return true;
}

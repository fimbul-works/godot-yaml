#include "packed_string_array_yaml.h"
#include "yaml.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

PackedStringArrayVariantConverter::PackedStringArrayVariantConverter(YAML* yaml) :
        VariantConverter(yaml) { }

void PackedStringArrayVariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  PackedStringArray array = v.operator PackedStringArray();
  node |= ryml::SEQ;
  for (const auto& s : array) {
    node.append_child() << s.utf8().get_data();
  }
}

Variant PackedStringArrayVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (node.is_seq()) {
    PackedStringArray array = PackedStringArray();
    int size = node.num_children();
    array.resize(size);
    for (int i = 0; i < size; ++i) {
      array[i] = String::utf8(node.child(i).val().str, node.child(i).val().len);
    }
    return array;
  }
  throw YAMLException("invalid PackedStringArray format - " + String::utf8(node.val().str, node.val().len));
}

bool PackedStringArrayVariantConverter::set_format(const String& format_str)
{
  return true;
}

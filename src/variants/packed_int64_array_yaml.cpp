#include "packed_int64_array_yaml.h"
#include "../util_numeric.h"
#include "../yaml_exception.h"

using namespace godot;

PackedInt64ArrayVariantConverter::PackedInt64ArrayVariantConverter(YAML* yaml) :
        VariantConverter(yaml) { }

void PackedInt64ArrayVariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  PackedInt64Array array = v.operator PackedInt64Array();
  emit_as_sequence(node, array);
}

void PackedInt64ArrayVariantConverter::emit_as_sequence(ryml::NodeRef& node, const PackedInt64Array& array) const
{
  node |= ryml::SEQ;

  if (array.size() == 0) {
    return; // Empty sequence
  }

  node |= ryml::FLOW_SL;

  for (int i = 0; i < array.size(); ++i) {
    node.append_child() << int_to_string(array[i]);
  }
}

Variant PackedInt64ArrayVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (!node.is_seq()) {
    throw YAMLException::create_invalid_format("PackedInt64Array");
  }

  PackedInt64Array array;
  int size = node.num_children();
  array.resize(size);

  for (int i = 0; i < size; ++i) {
    try {
      array.set(i, string_to_int<int64_t>(node[i].val()));
    } catch (const std::exception& e) {
      throw YAMLException(String("Failed to decode int64 at index ") + String::num_int64(i) + ": " + e.what());
    }
  }

  return array;
}

bool PackedInt64ArrayVariantConverter::set_format(const String& format_str)
{
  // PackedInt64Array only supports sequence format
  return true;
}

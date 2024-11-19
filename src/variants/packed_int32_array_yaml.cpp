#include "packed_int32_array_yaml.h"
#include "../util_numeric.h"
#include "../yaml_exception.h"

using namespace godot;

PackedInt32ArrayVariantConverter::PackedInt32ArrayVariantConverter(YAML* yaml) :
        VariantConverter(yaml) { }

void PackedInt32ArrayVariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  PackedInt32Array array = v.operator PackedInt32Array();
  emit_as_sequence(node, array);
}

void PackedInt32ArrayVariantConverter::emit_as_sequence(ryml::NodeRef& node, const PackedInt32Array& array) const
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

Variant PackedInt32ArrayVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (!node.is_seq()) {
    throw YAMLException::create_invalid_format("PackedInt32Array");
  }

  PackedInt32Array array;
  int size = node.num_children();
  array.resize(size);

  for (int i = 0; i < size; ++i) {
    try {
      array.set(i, string_to_int<int32_t>(node[i].val()));
    } catch (const std::exception& e) {
      throw YAMLException(String("Failed to decode int32 at index ") + String::num_int64(i) + ": " + e.what());
    }
  }

  return array;
}

bool PackedInt32ArrayVariantConverter::set_format(const String& format_str)
{
  // PackedInt32Array only supports sequence format
  return true;
}

#include "packed_float32_array_yaml.h"
#include "../util_numeric.h"
#include "../yaml_exception.h"

using namespace godot;

PackedFloat32ArrayVariantConverter::PackedFloat32ArrayVariantConverter(YAML* yaml) :
        VariantConverter(yaml) { }

void PackedFloat32ArrayVariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  PackedFloat32Array array = v.operator PackedFloat32Array();
  emit_as_sequence(node, array);
}

void PackedFloat32ArrayVariantConverter::emit_as_sequence(ryml::NodeRef& node, const PackedFloat32Array& array) const
{
  node |= ryml::SEQ;

  if (array.size() == 0) {
    return; // Empty sequence
  }

  node |= ryml::FLOW_SL;

  for (int i = 0; i < array.size(); ++i) {
    node.append_child() << float_to_string(array[i]);
  }
}

Variant PackedFloat32ArrayVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (!node.is_seq()) {
    throw YAMLException::create_invalid_format("PackedFloat32Array");
  }

  PackedFloat32Array array;
  int size = node.num_children();
  array.resize(size);

  for (int i = 0; i < size; ++i) {
    try {
      array.set(i, string_to_float<float>(node[i].val()));
    } catch (const std::exception& e) {
      throw YAMLException(String("Failed to decode float at index ") + String::num_int64(i) + ": " + e.what());
    }
  }

  return array;
}

bool PackedFloat32ArrayVariantConverter::set_format(const String& format_str)
{
  // PackedFloat32Array only supports sequence format
  return true;
}

#include "packed_float32_array_yaml.h"
#include "../exception.h"
#include "../util_numeric.h"

using namespace godot;

void PackedFloat32ArrayVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const
{
  const PackedFloat32Array array = v.operator PackedFloat32Array();
  emit_as_sequence(node, array, format);
}

void PackedFloat32ArrayVariantConverter::emit_as_sequence(
        ryml::NodeRef& node,
        const PackedFloat32Array& array,
        const YAMLFormat::View& format) const
{
  node |= ryml::SEQ;

  if (array.size() == 0) {
    return; // Empty sequence
  }

  if (format.get_format(Variant::PACKED_FLOAT32_ARRAY) == YAMLFormat::FLOW_SEQUENCE) {
    node |= ryml::FLOW_SL;
  }

  for (int i = 0; i < array.size(); ++i) {
    node.append_child() << float_to_string(array[i]);
  }
}

Variant PackedFloat32ArrayVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (!node.is_seq()) {
    throw YAMLException::create_invalid_format("PackedFloat32Array");
  }

  const size_t size = node.num_children();
  PackedFloat32Array array;
  array.resize(size);

  for (size_t i = 0; i < size; ++i) {
    try {
      array.set(i, string_to_float<float>(node[i].val()));
    } catch (const std::exception& e) {
      throw YAMLException::create_decode_error(String("float at index " + String::num_uint64(i)).utf8().get_data(), e.what());
    }
  }

  return array;
}

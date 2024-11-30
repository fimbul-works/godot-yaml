#include "packed_int64_array_yaml.h"
#include "../exception.h"
#include "../util_numeric.h"

using namespace godot;

void PackedInt64ArrayVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const
{
  const PackedInt64Array array = v.operator PackedInt64Array();
  emit_as_sequence(node, array, format);
}

void PackedInt64ArrayVariantConverter::emit_as_sequence(
        ryml::NodeRef& node,
        const PackedInt64Array& array,
        const YAMLFormat::View& format) const
{
  node |= ryml::SEQ;

  if (array.size() == 0) {
    return; // Empty sequence
  }

  if (format.get_format(Variant::PACKED_INT64_ARRAY) == YAMLFormat::FLOW_SEQUENCE) {
    node |= ryml::FLOW_SL;
  }

  for (int i = 0; i < array.size(); ++i) {
    node.append_child() << int_to_string(array[i]);
  }
}

Variant PackedInt64ArrayVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (!node.is_seq()) {
    throw YAMLException::create_invalid_format("PackedInt64Array");
  }

  const size_t size = node.num_children();
  PackedInt64Array array;
  array.resize(size);

  for (size_t i = 0; i < size; ++i) {
    try {
      array.set(i, string_to_int<int64_t>(node[i].val()));
    } catch (const std::exception& e) {
      throw YAMLException::create_decode_error(String("64-bit integer at index " + String::num_uint64(i)).utf8().get_data(), e.what());
    }
  }

  return array;
}

#include "packed_float64_array_yaml.h"
#include "../util_numeric.h"
#include "../yaml_exception.h"

using namespace godot;

PackedFloat64ArrayVariantConverter::PackedFloat64ArrayVariantConverter(YAML* yaml) :
        VariantConverter(yaml) { }

void PackedFloat64ArrayVariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  PackedFloat64Array array = v.operator PackedFloat64Array();
  emit_as_sequence(node, array);
}

void PackedFloat64ArrayVariantConverter::emit_as_sequence(ryml::NodeRef& node, const PackedFloat64Array& array) const
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

Variant PackedFloat64ArrayVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (!node.is_seq()) {
    throw YAMLException::create_invalid_format("PackedFloat64Array");
  }

  PackedFloat64Array array;
  int size = node.num_children();
  array.resize(size);

  for (int i = 0; i < size; ++i) {
    try {
      array.set(i, string_to_float<double>(node[i].val()));
    } catch (const std::exception& e) {
      throw YAMLException(String("Failed to decode double at index ") + String::num_int64(i) + ": " + e.what());
    }
  }

  return array;
}

bool PackedFloat64ArrayVariantConverter::set_format(const String& format_str)
{
  // PackedFloat64Array only supports sequence format
  return true;
}

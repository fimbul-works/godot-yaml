#include "packed_float64_array_yaml.h"
#include "../exception.h"
#include "../util_numeric.h"

using namespace godot;

void PackedFloat64ArrayVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const Ref<YAMLStyle>& style) const
{
  const PackedFloat64Array array = v.operator PackedFloat64Array();
  node |= ryml::SEQ;

  if (array.size() == 0) {
    return; // Empty sequence
  }

  // Apply flow style to the sequence if specified
  if (style.is_valid() && style->collection_style == YAMLStyle::COLLECTION_FLOW) {
    node |= ryml::FLOW_SL;
  }

  // Get shared item style if it exists
  Ref<YAMLStyle> shared_item_style;
  if (style.is_valid()) {
    shared_item_style = style->get_child("_items");
  }

  for (int i = 0; i < array.size(); ++i) {
    ryml::NodeRef value_node = node.append_child();

    // Check for individual item style, fall back to shared style
    Ref<YAMLStyle> item_style;
    if (style.is_valid()) {
      item_style = style->get_child(String::num_int64(i));
      if (!item_style.is_valid()) {
        item_style = shared_item_style;
      }
    }

    // Format number based on style
    if (item_style.is_valid()) {
      value_node << float_to_string(array[i], item_style->number_format);
    } else {
      value_node << float_to_string(array[i]);
    }
  }
}

Variant PackedFloat64ArrayVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (!node.is_seq()) {
    throw YAMLException::create_invalid_format("PackedFloat64Array");
  }

  const size_t size = node.num_children();
  PackedFloat64Array array;
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

#include "packed_int32_array_yaml.h"
#include "../exception.h"
#include "../util_numeric.h"

using namespace godot;

void PackedInt32ArrayVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const YAMLStyle::View& style) const
{
  const PackedInt32Array array = v.operator PackedInt32Array();
  node |= ryml::SEQ;

  if (array.size() == 0) {
    return; // Empty sequence
  }

  // Apply flow style to the sequence if specified
  style.apply_flow_style(node);

  // Get shared item style if it exists
  YAMLStyle::View shared_item_style;
  if (style.is_valid()) {
    shared_item_style = style.get_child("_items");
  }

  for (int i = 0; i < array.size(); ++i) {
    ryml::NodeRef value_node = node.append_child();

    YAMLStyle::View item_style;
    if (style.is_valid()) {
      // Check for individual item style
      const String idx = String::num_int64(i);
      if (style.has_child(idx)) {
        item_style = style.get_child(idx);
      }

      // Fall back to shared style
      if (!item_style.is_valid()) {
        item_style = shared_item_style;
      }
    }

    // Format number based on style
    if (item_style.is_valid()) {
      value_node << int_to_string(array[i], item_style.get_number_format());
    } else {
      value_node << int_to_string(array[i]);
    }
  }
}

Variant PackedInt32ArrayVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (!node.is_seq()) {
    throw YAMLException::create_invalid_format("PackedInt32Array");
  }

  const size_t size = node.num_children();
  PackedInt32Array array;
  array.resize(size);

  for (size_t i = 0; i < size; ++i) {
    try {
      array.set(i, static_cast<int32_t>(string_to_int<int64_t>(node[i].val())));
    } catch (const std::exception& e) {
      throw YAMLException::create_decode_error(String("integer at index " + String::num_uint64(i)).utf8().get_data(), e.what());
    }
  }

  return array;
}

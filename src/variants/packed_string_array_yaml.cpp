#include "packed_string_array_yaml.h"
#include "../exception.h"

using namespace godot;

void PackedStringArrayVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const Ref<YAMLStyle>& style) const
{
  const PackedStringArray array = v.operator PackedStringArray();
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
    const String& str = array[i];
    ryml::NodeRef child = node.append_child();

    if (str.is_empty()) {
      ryml::csubstr empty = {};
      child << empty;
      continue;
    }

    // Check for individual item style, fall back to shared style
    Ref<YAMLStyle> item_style;
    if (style.is_valid()) {
      item_style = style->get_child(String::num_int64(i));
      if (!item_style.is_valid()) {
        item_style = shared_item_style;
      }
    }

    // Apply string styling
    if (item_style.is_valid()) {
      switch (item_style->scalar_style) {
        case YAMLStyle::STYLE_BLOCK:
          child |= ryml::BLOCK;
          if (item_style->block_style == YAMLStyle::BLOCK_FOLDED) {
            child |= ryml::VAL_FOLDED;
          }
          break;
        case YAMLStyle::STYLE_QUOTED:
          child |= ryml::VAL_DQUO; // or SQUO for single quotes if you prefer
          break;
        case YAMLStyle::STYLE_PLAIN:
          // Let YAML decide if quoting is needed
          if (needs_block_style(str)) {
            child |= ryml::BLOCK;
          }
          break;
        default:
          if (needs_block_style(str)) {
            child |= ryml::BLOCK;
          }
          break;
      }
    } else {
      // Default behavior
      if (needs_block_style(str)) {
        child |= ryml::BLOCK;
      }
    }

    child << str.utf8().get_data();
  }
}

Variant PackedStringArrayVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (!node.is_seq()) {
    throw YAMLException::create_invalid_format("PackedStringArray");
  }

  const size_t size = node.num_children();
  PackedStringArray array;
  array.resize(size);

  for (size_t i = 0; i < size; ++i) {
    try {
      const ryml::ConstNodeRef& child = node[i];
      if (child.val_is_null() || !child.has_val()) {
        array.set(i, String());
      } else {
        array.set(i, from_ryml_str(child.val()));
      }
    } catch (const std::exception& e) {
      throw YAMLException::create_decode_error(String("string at index " + String::num_uint64(i)).utf8().get_data(), e.what());
    }
  }

  return array;
}

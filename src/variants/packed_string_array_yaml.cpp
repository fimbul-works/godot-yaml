#include "packed_string_array_yaml.h"
#include "../yaml_exception.h"

using namespace godot;

void PackedStringArrayVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const
{
  const PackedStringArray array = v.operator PackedStringArray();
  emit_as_sequence(node, array, format);
}

void PackedStringArrayVariantConverter::emit_as_sequence(
        ryml::NodeRef& node,
        const PackedStringArray& array,
        const YAMLFormat::View& format) const
{
  node |= ryml::SEQ;

  if (array.size() == 0) {
    return; // Empty sequence
  }

  // Check if any string needs block style
  bool needs_block = false;
  if (format.get_format(Variant::PACKED_STRING_ARRAY) != YAMLFormat::FLOW_MAP) {
    for (int i = 0; i < array.size(); ++i) {
      if (needs_block_style(array[i])) {
        needs_block = true;
        break;
      }
    }
  }

  if (!needs_block) {
    node |= ryml::FLOW_SL;
  }

  for (int i = 0; i < array.size(); ++i) {
    const String& str = array[i];
    if (str.is_empty()) {
      // Empty string is represented as empty scalar
      ryml::NodeRef child = node.append_child();
      ryml::csubstr empty = {};
      child << empty;
    } else {
      // Use block style for strings with newlines or if requested
      ryml::NodeRef child = node.append_child();
      if (needs_block_style(str)) {
        child |= ryml::BLOCK;
      }
      child << str.utf8().get_data();
    }
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
      throw YAMLException(String("Failed to decode string at index ") + String::num_uint64(i) + ": " + e.what());
    }
  }

  return array;
}

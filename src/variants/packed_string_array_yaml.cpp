#include "packed_string_array_yaml.h"
#include "../yaml_exception.h"

using namespace godot;

PackedStringArrayVariantConverter::PackedStringArrayVariantConverter(YAML* yaml) :
        VariantConverter(yaml) { }

void PackedStringArrayVariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  PackedStringArray array = v.operator PackedStringArray();
  emit_as_sequence(node, array);
}

void PackedStringArrayVariantConverter::emit_as_sequence(ryml::NodeRef& node, const PackedStringArray& array) const
{
  node |= ryml::SEQ;

  if (array.size() == 0) {
    return; // Empty sequence
  }

  // Note: We don't use FLOW_SL for strings by default as they might contain newlines
  // or be more readable in block format

  for (int i = 0; i < array.size(); ++i) {
    const String& str = array[i];
    if (str.is_empty()) {
      // Empty string is represented as empty scalar
      ryml::NodeRef child = node.append_child();
      ryml::csubstr empty = {};
      child << empty;
    } else {
      node.append_child() << str.utf8().get_data();
    }
  }
}

Variant PackedStringArrayVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (!node.is_seq()) {
    throw YAMLException::create_invalid_format("PackedStringArray");
  }

  PackedStringArray array;
  int size = node.num_children();
  array.resize(size);

  for (int i = 0; i < size; ++i) {
    try {
      const ryml::ConstNodeRef& child = node[i];
      if (child.val_is_null() || !child.has_val()) {
        array.set(i, String()); // Empty string
      } else {
        array.set(i, String::utf8(child.val().str, child.val().len));
      }
    } catch (const std::exception& e) {
      throw YAMLException(String("Failed to decode string at index ") + String::num_int64(i) + ": " + e.what());
    }
  }

  return array;
}

bool PackedStringArrayVariantConverter::set_format(const String& format_str)
{
  // PackedStringArray only supports sequence format
  return true;
}

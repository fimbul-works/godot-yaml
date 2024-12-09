#include "packed_color_array_yaml.h"
#include "../exception.h"
#include "../variant_converter_registry.h"

using namespace godot;

void PackedColorArrayVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const YAMLStyle::View& style) const
{
  const PackedColorArray array = v.operator PackedColorArray();
  node |= ryml::SEQ;

  if (array.size() == 0) {
    return; // Empty sequence
  }

  // Apply flow style to the sequence itself if specified
  style.apply_flow_style(node);

  const auto* color_converter = VariantConverterRegistry::get_instance().get_converter(Variant::COLOR);

  // Get shared item style if it exists (key "_items" is a convention for shared array item styling)
  YAMLStyle::View shared_item_style;
  if (style.is_valid()) {
    shared_item_style = style.get_child("_items");
  }

  for (int i = 0; i < array.size(); ++i) {
    ryml::NodeRef color_node = node.append_child();

    // Check for individual item style, fall back to shared style
    YAMLStyle::View item_style;
    if (style.is_valid()) {
      item_style = style.get_child(String::num_int64(i));
      if (!item_style.is_valid()) {
        item_style = shared_item_style;
      }
    }

    color_converter->encode(color_node, array[i], item_style);
  }
}

Variant PackedColorArrayVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (!node.is_seq()) {
    throw YAMLException::create_invalid_format("PackedColorArray");
  }

  PackedColorArray array;
  const size_t size = node.num_children();
  array.resize(size);

  if (size > 0) {
    const auto* color_converter = VariantConverterRegistry::get_instance().get_converter(Variant::COLOR);

    for (size_t i = 0; i < size; ++i) {
      try {
        Color color = color_converter->decode(node[i]);
        array.set(i, color);
      } catch (const std::exception& e) {
        throw YAMLException::create_decode_error(String("Color at index " + String::num_uint64(i)).utf8().get_data(), e.what());
      }
    }
  }

  return array;
}

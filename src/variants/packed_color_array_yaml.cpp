#include "packed_color_array_yaml.h"
#include "../variant_converter_registry.h"
#include "../yaml_exception.h"

using namespace godot;

void PackedColorArrayVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const
{
  const PackedColorArray array = v.operator PackedColorArray();
  node |= ryml::SEQ;

  if (array.size() == 0) {
    return; // Empty sequence
  }

  node |= ryml::FLOW_SL;
  const auto* color_converter = VariantConverterRegistry::get_converter(Variant::COLOR);

  for (int i = 0; i < array.size(); ++i) {
    ryml::NodeRef color_node = node.append_child();
    color_converter->encode(color_node, array[i], format);
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
    const auto* color_converter = VariantConverterRegistry::get_converter(Variant::COLOR);

    for (size_t i = 0; i < size; ++i) {
      try {
        Color color = color_converter->decode(node[i]);
        array.set(i, color);
      } catch (const std::exception& e) {
        throw YAMLException(String("Failed to decode color at index ") + String::num_uint64(i) + ": " + e.what());
      }
    }
  }

  return array;
}

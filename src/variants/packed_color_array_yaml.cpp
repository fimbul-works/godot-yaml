#include "packed_color_array_yaml.h"
#include "../yaml_exception.h"

using namespace godot;

PackedColorArrayVariantConverter::PackedColorArrayVariantConverter(YAML* yaml) :
        VariantConverter(yaml)
{
  color_encoder = new ColorVariantConverter(yaml);
  color_encoder->set_format("flow"); // Use flow format for individual colors
}

PackedColorArrayVariantConverter::~PackedColorArrayVariantConverter()
{
  delete color_encoder;
}

void PackedColorArrayVariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  PackedColorArray array = v.operator PackedColorArray();

  if (array.size() == 0) {
    // Empty array is represented as empty sequence
    node |= ryml::SEQ;
    return;
  }

  node |= ryml::SEQ;
  node |= ryml::FLOW_SL;

  for (int i = 0; i < array.size(); ++i) {
    ryml::NodeRef color_node = node.append_child();
    color_encoder->encode(color_node, array[i]);
  }
}

Variant PackedColorArrayVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (!node.is_seq()) {
    throw YAMLException::create_invalid_format("PackedColorArray");
  }

  PackedColorArray array;
  int size = node.num_children();
  array.resize(size);

  for (int i = 0; i < size; ++i) {
    try {
      Color color = color_encoder->decode(node[i]);
      array.set(i, color);
    } catch (const std::exception& e) {
      throw YAMLException(String("Failed to decode color at index ") + String::num_int64(i) + ": " + e.what());
    }
  }

  return array;
}

bool PackedColorArrayVariantConverter::set_format(const String& format_str)
{
  // Format setting is delegated to the color encoder
  return color_encoder->set_format(format_str);
}

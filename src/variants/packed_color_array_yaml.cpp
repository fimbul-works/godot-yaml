#include "packed_color_array_yaml.h"
#include "yaml.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

PackedColorArrayVariantConverter::PackedColorArrayVariantConverter(YAML* yaml) :
        VariantConverter(yaml)
{
  color_encoder = new ColorVariantConverter(yaml);
  color_encoder->set_format("flow");
}

PackedColorArrayVariantConverter::~PackedColorArrayVariantConverter()
{
  delete color_encoder;
}

void PackedColorArrayVariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  PackedColorArray array = v.operator PackedColorArray();
  node |= ryml::SEQ;
  for (const auto& color : array) {
    ryml::NodeRef color_node = node.append_child();
    color_encoder->encode(color_node, color);
  }
}

Variant PackedColorArrayVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (node.is_seq()) {
    PackedColorArray array = PackedColorArray();
    int size = node.num_children();
    array.resize(size);
    for (int i = 0; i < size; ++i) {
      array[i] = color_encoder->decode(node.child(i));
    }
    return array;
  }
  throw YAMLException("invalid PackedColorArray format - " + String::utf8(node.val().str, node.val().len));
}

bool PackedColorArrayVariantConverter::set_format(const String& format_str)
{
  return color_encoder->set_format(format_str);
}

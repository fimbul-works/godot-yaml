#include "packed_color_array_yaml.h"
#include "yaml.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

PackedColorArrayYAMLEncoder::PackedColorArrayYAMLEncoder(YAML* yaml) :
        YAMLEncoder(yaml)
{
  color_encoder = new ColorYAMLEncoder(yaml);
  color_encoder->set_format("flow");
}

PackedColorArrayYAMLEncoder::~PackedColorArrayYAMLEncoder()
{
  delete color_encoder;
}

void PackedColorArrayYAMLEncoder::encode(ryml::NodeRef& node, const Variant& v) const
{
  PackedColorArray array = v.operator PackedColorArray();
  node |= ryml::SEQ;
  for (const auto& color : array) {
    ryml::NodeRef color_node = node.append_child();
    color_encoder->encode(color_node, color);
  }
}

Variant PackedColorArrayYAMLEncoder::decode(const ryml::ConstNodeRef& node) const
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

bool PackedColorArrayYAMLEncoder::set_format(const String& format_str)
{
  return color_encoder->set_format(format_str);
}

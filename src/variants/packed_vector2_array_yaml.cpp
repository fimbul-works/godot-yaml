#include "packed_vector2_array_yaml.h"
#include "yaml.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

PackedVector2ArrayVariantConverter::PackedVector2ArrayVariantConverter(YAML* yaml) :
        VariantConverter(yaml)
{
  vec_encoder = new Vector2VariantConverter(yaml);
  vec_encoder->set_format("flow");
}

PackedVector2ArrayVariantConverter::~PackedVector2ArrayVariantConverter()
{
  delete vec_encoder;
}

void PackedVector2ArrayVariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  PackedVector2Array array = v.operator PackedVector2Array();
  node |= ryml::SEQ;
  for (const auto& color : array) {
    ryml::NodeRef color_node = node.append_child();
    vec_encoder->encode(color_node, color);
  }
}

Variant PackedVector2ArrayVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (node.is_seq()) {
    PackedVector2Array array = PackedVector2Array();
    int size = node.num_children();
    array.resize(size);
    for (int i = 0; i < size; ++i) {
      array[i] = vec_encoder->decode(node.child(i));
    }
    return array;
  }
  throw YAMLException("invalid PackedVector2Array format - " + String::utf8(node.val().str, node.val().len));
}

bool PackedVector2ArrayVariantConverter::set_format(const String& format_str)
{
  return vec_encoder->set_format(format_str);
}

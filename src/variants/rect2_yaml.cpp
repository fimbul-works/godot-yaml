#include "rect2_yaml.h"
#include "yaml.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

Rect2VariantConverter::Rect2VariantConverter(YAML* yaml) :
        VariantConverter(yaml)
{
  vec_encoder = new Vector2VariantConverter(yaml);
  vec_encoder->set_format("flow");
}

Rect2VariantConverter::~Rect2VariantConverter()
{
  delete vec_encoder;
}

void Rect2VariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  Rect2 rect = v.operator Rect2();
  emit_as_map(node, rect);
}

Variant Rect2VariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (node.is_map() && node.has_child("position") && node.has_child("size")) {
    Vector2 position = vec_encoder->decode(node["position"]).operator Vector2();
    Vector2 size = vec_encoder->decode(node["size"]).operator Vector2();
    return Rect2(position, size);
  }
  throw YAMLException("invalid Rect2 format - " + String::utf8(node.val().str, node.val().len));
}

bool Rect2VariantConverter::set_format(const String& format_str)
{
  return vec_encoder->set_format(format_str);
}

void Rect2VariantConverter::emit_as_map(ryml::NodeRef& node, const Rect2& rect) const
{
  node |= ryml::MAP;

  ryml::NodeRef position_node = node["position"];
  vec_encoder->encode(position_node, rect.position);

  ryml::NodeRef size_node = node["size"];
  vec_encoder->encode(size_node, rect.size);

  node["position"] |= ryml::FLOW_SL;
  node["size"] |= ryml::FLOW_SL;
}

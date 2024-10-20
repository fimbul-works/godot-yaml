#include "rect2_yaml.h"
#include "vector2_yaml.h"
#include "yaml.h"
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

Rect2YAMLEncoder::Rect2YAMLEncoder()
{
  vec2_encoder.set_format("flow");
}

const char* Rect2YAMLEncoder::get_tag() const
{
  return "Rect2";
}

void Rect2YAMLEncoder::encode(ryml::NodeRef& node, const Variant& v) const
{
  Rect2 rect = v.operator Rect2();
  emit_as_map(node, rect);
}

Variant Rect2YAMLEncoder::decode(const ryml::ConstNodeRef& node) const
{
  if (node.is_map() && node.has_child("position") && node.has_child("size")) {
    Vector2 position = vec2_encoder.decode(node["position"]).operator Vector2();
    Vector2 size = vec2_encoder.decode(node["size"]).operator Vector2();

    return Rect2(position, size);
  }
  throw YAMLException("invalid Rect2 format - " + String::utf8(node.val().str, node.val().len));
}

bool Rect2YAMLEncoder::set_format(const String& format_str)
{
  return vec2_encoder.set_format(format_str);
}

void Rect2YAMLEncoder::emit_as_map(ryml::NodeRef& node, const Rect2& rect) const
{
  node |= ryml::MAP;

  ryml::NodeRef position_node = node["position"];
  vec2_encoder.encode(position_node, rect.position);

  ryml::NodeRef size_node = node["size"];
  vec2_encoder.encode(size_node, rect.size);

  node["position"] |= ryml::FLOW_SL;
  node["size"] |= ryml::FLOW_SL;
}

#include "rect2i_yaml.h"
#include "yaml.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

Rect2iYAMLEncoder::Rect2iYAMLEncoder(YAML* yaml) :
        YAMLEncoder(yaml)
{
  vec_encoder = new Vector2iYAMLEncoder(yaml);
  vec_encoder->set_format("flow");
}

Rect2iYAMLEncoder::~Rect2iYAMLEncoder()
{
  delete vec_encoder;
}

void Rect2iYAMLEncoder::encode(ryml::NodeRef& node, const Variant& v) const
{
  Rect2i rect = v.operator Rect2i();
  emit_as_map(node, rect);
}

Variant Rect2iYAMLEncoder::decode(const ryml::ConstNodeRef& node) const
{
  if (node.is_map() && node.has_child("position") && node.has_child("size")) {
    Vector2i position = vec_encoder->decode(node["position"]).operator Vector2i();
    Vector2i size = vec_encoder->decode(node["size"]).operator Vector2i();
    return Rect2i(position, size);
  }
  throw YAMLException("invalid Rect2i format - " + String::utf8(node.val().str, node.val().len));
}

bool Rect2iYAMLEncoder::set_format(const String& format_str)
{
  return vec_encoder->set_format(format_str);
}

void Rect2iYAMLEncoder::emit_as_map(ryml::NodeRef& node, const Rect2i& rect) const
{
  node |= ryml::MAP;

  ryml::NodeRef position_node = node["position"];
  vec_encoder->encode(position_node, rect.position);

  ryml::NodeRef size_node = node["size"];
  vec_encoder->encode(size_node, rect.size);

  node["position"] |= ryml::FLOW_SL;
  node["size"] |= ryml::FLOW_SL;
}

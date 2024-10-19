#include "rect2i_yaml.h"
#include "variants/vector2i_yaml.h"
#include "yaml.h"
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

Rect2iYAMLEncoder::Rect2iYAMLEncoder(YAML* yaml) :
        m_yaml(yaml) { }

const char* Rect2iYAMLEncoder::get_tag() const
{
  return "!!Rect2i";
}

void Rect2iYAMLEncoder::encode(ryml::NodeRef& node, const Variant& v) const
{
  Rect2i rect = v.operator Rect2i();
  emit_as_map(node, rect);
}

Variant Rect2iYAMLEncoder::decode(const ryml::ConstNodeRef& node) const
{
  if (node.is_map() && node.has_child("position") && node.has_child("size")) {
    const Vector2iYAMLEncoder* vector2i_encoder = static_cast<const Vector2iYAMLEncoder*>(m_yaml->get_encoder(Variant::VECTOR2I));
    if (!vector2i_encoder) {
      UtilityFunctions::printerr("Vector2i encoder not found");
      return Variant();
    }

    Vector2i position = vector2i_encoder->decode(node["position"]).operator Vector2i();
    Vector2i size = vector2i_encoder->decode(node["size"]).operator Vector2i();

    return Rect2i(position, size);
  }

  UtilityFunctions::printerr("Invalid Rect2i format");
  return Variant();
}

bool Rect2iYAMLEncoder::set_format(const String& format_str)
{
  return true;
}

void Rect2iYAMLEncoder::emit_as_map(ryml::NodeRef& node, const Rect2i& rect) const
{
  const Vector2iYAMLEncoder* vector2i_encoder = static_cast<const Vector2iYAMLEncoder*>(m_yaml->get_encoder(Variant::VECTOR2I));
  if (!vector2i_encoder) {
    UtilityFunctions::printerr("Vector2i encoder not found");
    return;
  }

  node |= ryml::MAP;

  ryml::NodeRef position_node = node["position"];
  vector2i_encoder->encode(position_node, rect.position);

  ryml::NodeRef size_node = node["size"];
  vector2i_encoder->encode(size_node, rect.size);

  node["position"] |= ryml::FLOW_SL;
  node["size"] |= ryml::FLOW_SL;
}

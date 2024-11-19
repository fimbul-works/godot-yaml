#include "rect2i_yaml.h"
#include "../yaml_exception.h"

using namespace godot;

Rect2iVariantConverter::Rect2iVariantConverter(YAML* yaml) :
        VariantConverter(yaml)
{
  vec_encoder = new Vector2iVariantConverter(yaml);
  vec_encoder->set_format("flow");
}

Rect2iVariantConverter::~Rect2iVariantConverter()
{
  delete vec_encoder;
}

void Rect2iVariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  Rect2i rect = v.operator Rect2i();
  emit_as_map(node, rect);
}

void Rect2iVariantConverter::emit_as_map(ryml::NodeRef& node, const Rect2i& rect) const
{
  node |= ryml::MAP;
  node |= ryml::FLOW_SL;

  ryml::NodeRef position_node = node["position"];
  vec_encoder->encode(position_node, rect.position);

  ryml::NodeRef size_node = node["size"];
  vec_encoder->encode(size_node, rect.size);
}

Variant Rect2iVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (!node.is_map()) {
    throw YAMLException::create_invalid_format("Rect2i");
  }

  if (!node.has_child("position")) {
    throw YAMLException::create_missing_field("Rect2i", "position");
  }
  if (!node.has_child("size")) {
    throw YAMLException::create_missing_field("Rect2i", "size");
  }

  try {
    Vector2i position = vec_encoder->decode(node["position"]);
    Vector2i size = vec_encoder->decode(node["size"]);
    return Rect2i(position, size);
  } catch (const std::exception& e) {
    throw YAMLException(String("Failed to decode Rect2i: ") + e.what());
  }
}

bool Rect2iVariantConverter::set_format(const String& format_str)
{
  return vec_encoder->set_format(format_str);
}

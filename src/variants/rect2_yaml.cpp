#include "rect2_yaml.h"
#include "../yaml_exception.h"

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

void Rect2VariantConverter::emit_as_map(ryml::NodeRef& node, const Rect2& rect) const
{
  node |= ryml::MAP;
  node |= ryml::FLOW_SL;

  ryml::NodeRef position_node = node["position"];
  vec_encoder->encode(position_node, rect.position);

  ryml::NodeRef size_node = node["size"];
  vec_encoder->encode(size_node, rect.size);
}

Variant Rect2VariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (!node.is_map()) {
    throw YAMLException::create_invalid_format("Rect2");
  }

  if (!node.has_child("position")) {
    throw YAMLException::create_missing_field("Rect2", "position");
  }
  if (!node.has_child("size")) {
    throw YAMLException::create_missing_field("Rect2", "size");
  }

  try {
    Vector2 position = vec_encoder->decode(node["position"]);
    Vector2 size = vec_encoder->decode(node["size"]);
    return Rect2(position, size);
  } catch (const std::exception& e) {
    throw YAMLException(String("Failed to decode Rect2: ") + e.what());
  }
}

bool Rect2VariantConverter::set_format(const String& format_str)
{
  return vec_encoder->set_format(format_str);
}

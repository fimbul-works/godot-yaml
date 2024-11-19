#include "aabb_yaml.h"
#include "../yaml_exception.h"

using namespace godot;

AABBVariantConverter::AABBVariantConverter(YAML* yaml) :
        VariantConverter(yaml)
{
  vec_encoder = new Vector3VariantConverter(yaml);
  vec_encoder->set_format("flow"); // Always use flow format for the Vector3 components
}

AABBVariantConverter::~AABBVariantConverter()
{
  delete vec_encoder;
}

void AABBVariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  AABB aabb = v.operator AABB();
  emit_as_map(node, aabb);
}

Variant AABBVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (!node.is_map()) {
    throw YAMLException::create_invalid_format("AABB");
  }

  if (!node.has_child("position")) {
    throw YAMLException::create_missing_field("AABB", "position");
  }
  if (!node.has_child("size")) {
    throw YAMLException::create_missing_field("AABB", "size");
  }

  Vector3 position = vec_encoder->decode(node["position"]).operator Vector3();
  Vector3 size = vec_encoder->decode(node["size"]).operator Vector3();

  return AABB(position, size);
}

bool AABBVariantConverter::set_format(const String& format)
{
  // AABB only supports one format (map), but we delegate format setting to the Vector3 encoder
  return vec_encoder->set_format(format);
}

void AABBVariantConverter::emit_as_map(ryml::NodeRef& node, const AABB& aabb) const
{
  node |= ryml::MAP;
  node |= ryml::FLOW_SL;

  ryml::NodeRef position_node = node["position"];
  vec_encoder->encode(position_node, aabb.position);

  ryml::NodeRef size_node = node["size"];
  vec_encoder->encode(size_node, aabb.size);
}

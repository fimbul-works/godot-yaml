#include "aabb_yaml.h"
#include "../variant_converter_registry.h"
#include "../yaml_exception.h"

namespace godot {

void AABBVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const
{
  const AABB aabb = AABB(v);

  // Check format and use appropriate encoding method
  switch (format.get_format(Variant::AABB)) {
    case YAMLFormat::SEQUENCE:
      emit_as_sequence(node, aabb, format);
      break;
    case YAMLFormat::BLOCK_MAP:
    case YAMLFormat::FLOW_MAP:
    default:
      emit_as_map(node, aabb, format);
      break;
  }
}

void AABBVariantConverter::emit_as_map(ryml::NodeRef& node, const AABB& aabb, const YAMLFormat::View& format) const
{
  node |= ryml::MAP;
  node |= ryml::FLOW_SL;

  const auto* vec3_converter = get_vec3_converter();

  ryml::NodeRef position_node = node["position"];
  vec3_converter->encode(position_node, aabb.position, format);

  ryml::NodeRef size_node = node["size"];
  vec3_converter->encode(size_node, aabb.size, format);
}

void AABBVariantConverter::emit_as_sequence(ryml::NodeRef& node, const AABB& aabb, const YAMLFormat::View& format) const
{
  node |= ryml::SEQ;
  node |= ryml::FLOW_SL;

  const auto* vec3_converter = get_vec3_converter();

  ryml::NodeRef position_node = node.append_child();
  vec3_converter->encode(position_node, aabb.position, format);

  ryml::NodeRef size_node = node.append_child();
  vec3_converter->encode(size_node, aabb.size, format);
}

Variant AABBVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (node.is_map()) {
    return decode_from_map(node);
  } else if (node.is_seq()) {
    return decode_from_sequence(node);
  }
  throw YAMLException("Invalid AABB format: expected map or sequence");
}

Variant AABBVariantConverter::decode_from_map(const ryml::ConstNodeRef& node) const
{
  const auto position_node = node["position"];
  const auto size_node = node["size"];

  if (!position_node.valid()) {
    throw YAMLException("Missing required field 'position' in AABB");
  }
  if (!size_node.valid()) {
    throw YAMLException("Missing required field 'size' in AABB");
  }

  const auto* vec3_converter = get_vec3_converter();
  const Vector3 position = vec3_converter->decode(position_node).operator Vector3();
  const Vector3 size = vec3_converter->decode(size_node).operator Vector3();

  return AABB(position, size);
}

Variant AABBVariantConverter::decode_from_sequence(const ryml::ConstNodeRef& node) const
{
  if (node.num_children() != 2) {
    throw YAMLException("Invalid AABB sequence: expected exactly 2 elements");
  }

  const auto* vec3_converter = get_vec3_converter();
  const Vector3 position = vec3_converter->decode(node[0]).operator Vector3();
  const Vector3 size = vec3_converter->decode(node[1]).operator Vector3();

  return AABB(position, size);
}

const VariantConverter* AABBVariantConverter::get_vec3_converter() const
{
  const auto* vec3_converter = VariantConverterRegistry::get_converter(Variant::VECTOR3);
  if (!vec3_converter) {
    throw YAMLException("Vector3 converter not found in registry");
  }
  return vec3_converter;
}

} // namespace godot

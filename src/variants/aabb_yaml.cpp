#include "aabb_yaml.h"
#include "vector3_yaml.h"
#include "yaml.h"
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

AABBYAMLEncoder::AABBYAMLEncoder()
{
  vec3_encoder.set_format("flow");
}

const char* AABBYAMLEncoder::get_tag() const
{
  return "AABB";
}

void AABBYAMLEncoder::encode(ryml::NodeRef& node, const Variant& v) const
{
  AABB aabb = v.operator godot::AABB();
  emit_as_map(node, aabb);
}

Variant AABBYAMLEncoder::decode(const ryml::ConstNodeRef& node) const
{
  if (node.is_map() && node.has_child("position") && node.has_child("size")) {
    Vector3 position = vec3_encoder.decode(node["position"]).operator Vector3();
    Vector3 size = vec3_encoder.decode(node["size"]).operator Vector3();
    return AABB(position, size);
  }

  throw YAMLException("invalid AABB format - " + String::utf8(node.val().str, node.val().len));
}

bool AABBYAMLEncoder::set_format(const String& format_str)
{
  return vec3_encoder.set_format(format_str);
}

void AABBYAMLEncoder::emit_as_map(ryml::NodeRef& node, const AABB& rect) const
{
  node |= ryml::MAP;

  ryml::NodeRef position_node = node["position"];
  vec3_encoder.encode(position_node, rect.position);

  ryml::NodeRef size_node = node["size"];
  vec3_encoder.encode(size_node, rect.size);
}

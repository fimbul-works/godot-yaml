#include "plane_yaml.h"
#include "util_numeric.h"
#include "yaml.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

PlaneYAMLEncoder::PlaneYAMLEncoder(YAML* yaml) :
        YAMLEncoder(yaml)
{
  vec_encoder = new Vector3YAMLEncoder(yaml);
  vec_encoder->set_format("flow");
}

PlaneYAMLEncoder::~PlaneYAMLEncoder()
{
  delete vec_encoder;
}

void PlaneYAMLEncoder::encode(ryml::NodeRef& node, const Variant& v) const
{
  Plane plane = v.operator godot::Plane();
  emit_as_map(node, plane);
}

Variant PlaneYAMLEncoder::decode(const ryml::ConstNodeRef& node) const
{
  if (node.is_map() && node.has_child("normal") && node.has_child("d")) {
    Vector3 normal = vec_encoder->decode(node["normal"]).operator Vector3();
    real_t d = string_to_float<real_t>(node["d"].val());
    return Plane(normal, d);
  }
  throw YAMLException("invalid Plane format - " + String::utf8(node.val().str, node.val().len));
}

bool PlaneYAMLEncoder::set_format(const String& format_str)
{
  return vec_encoder->set_format(format_str);
}

void PlaneYAMLEncoder::emit_as_map(ryml::NodeRef& node, const Plane& plane) const
{
  node |= ryml::MAP;

  ryml::NodeRef normal_node = node["normal"];
  vec_encoder->encode(normal_node, plane.normal);

  node["d"] << float_to_string(plane.d);
}

#include "transform3d_yaml.h"
#include "yaml.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

Transform3DVariantConverter::Transform3DVariantConverter(YAML* yaml) :
        VariantConverter(yaml)
{
  vec_encoder = new Vector3VariantConverter(yaml);
  vec_encoder->set_format("flow");
}

Transform3DVariantConverter::~Transform3DVariantConverter()
{
  delete vec_encoder;
}

void Transform3DVariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  Transform3D basis = v.operator Transform3D();
  emit_as_map(node, basis);
}

Variant Transform3DVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (node.is_map() && node.has_child("x") && node.has_child("y") && node.has_child("z") && node.has_child("origin")) {
    Vector3 x = vec_encoder->decode(node["x"]).operator Vector3();
    Vector3 y = vec_encoder->decode(node["y"]).operator Vector3();
    Vector3 z = vec_encoder->decode(node["z"]).operator Vector3();
    Vector3 origin = vec_encoder->decode(node["origin"]).operator Vector3();
    return Transform3D(x, y, z, origin);
  }
  throw YAMLException("invalid Transform3D format " + String::utf8(node.val().str, node.val().len));
}

bool Transform3DVariantConverter::set_format(const String& format_str)
{
  return vec_encoder->set_format(format_str);
}

void Transform3DVariantConverter::emit_as_map(ryml::NodeRef& node, const Transform3D& transform) const
{
  node |= ryml::MAP;

  ryml::NodeRef x_node = node["x"];
  vec_encoder->encode(x_node, transform.basis.get_column(0));

  ryml::NodeRef y_node = node["y"];
  vec_encoder->encode(y_node, transform.basis.get_column(1));

  ryml::NodeRef z_node = node["z"];
  vec_encoder->encode(z_node, transform.basis.get_column(2));

  ryml::NodeRef origin_node = node["origin"];
  vec_encoder->encode(origin_node, transform.origin);
}

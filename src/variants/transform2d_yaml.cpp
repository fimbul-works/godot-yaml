#include "transform2d_yaml.h"
#include "yaml.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

Transform2DVariantConverter::Transform2DVariantConverter(YAML* yaml) :
        VariantConverter(yaml)
{
  vec_encoder = new Vector2VariantConverter(yaml);
  vec_encoder->set_format("flow");
}

Transform2DVariantConverter::~Transform2DVariantConverter()
{
  delete vec_encoder;
}

void Transform2DVariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  Transform2D basis = v.operator Transform2D();
  emit_as_map(node, basis);
}

Variant Transform2DVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (node.is_map() && node.has_child("x") && node.has_child("y") && node.has_child("z")) {
    Vector2 x = vec_encoder->decode(node["x"]).operator Vector2();
    Vector2 y = vec_encoder->decode(node["y"]).operator Vector2();
    Vector2 z = vec_encoder->decode(node["z"]).operator Vector2();
    return Transform2D(x, y, z);
  }
  throw YAMLException("invalid Transform2D format - " + String::utf8(node.val().str, node.val().len));
}

bool Transform2DVariantConverter::set_format(const String& format_str)
{
  return vec_encoder->set_format(format_str);
}

void Transform2DVariantConverter::emit_as_map(ryml::NodeRef& node, const Transform2D& transform) const
{
  node |= ryml::MAP;

  ryml::NodeRef x_node = node["x"];
  vec_encoder->encode(x_node, transform.columns[0]);

  ryml::NodeRef y_node = node["y"];
  vec_encoder->encode(y_node, transform.columns[1]);

  ryml::NodeRef z_node = node["z"];
  vec_encoder->encode(z_node, transform.columns[2]);
}

#include "projection_yaml.h"
#include "yaml.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

ProjectionVariantConverter::ProjectionVariantConverter(YAML* yaml) :
        VariantConverter(yaml)
{
  vec_encoder = new Vector4VariantConverter(yaml);
  vec_encoder->set_format("flow");
}

ProjectionVariantConverter::~ProjectionVariantConverter()
{
  delete vec_encoder;
}

void ProjectionVariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  Projection basis = v.operator Projection();
  emit_as_map(node, basis);
}

Variant ProjectionVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (node.is_map() && node.has_child("x") && node.has_child("y") && node.has_child("z") && node.has_child("w")) {
    Vector4 x = vec_encoder->decode(node["x"]).operator Vector4();
    Vector4 y = vec_encoder->decode(node["y"]).operator Vector4();
    Vector4 z = vec_encoder->decode(node["z"]).operator Vector4();
    Vector4 w = vec_encoder->decode(node["w"]).operator Vector4();
    return Projection(x, y, z, w);
  }
  throw YAMLException("invalid Projection format - " + String::utf8(node.val().str, node.val().len));
}

bool ProjectionVariantConverter::set_format(const String& format_str)
{
  return vec_encoder->set_format(format_str);
}

void ProjectionVariantConverter::emit_as_map(ryml::NodeRef& node, const Projection& projection) const
{
  node |= ryml::MAP;

  ryml::NodeRef x_node = node["x"];
  vec_encoder->encode(x_node, projection.columns[0]);

  ryml::NodeRef y_node = node["y"];
  vec_encoder->encode(y_node, projection.columns[1]);

  ryml::NodeRef z_node = node["z"];
  vec_encoder->encode(z_node, projection.columns[2]);

  ryml::NodeRef w_node = node["w"];
  vec_encoder->encode(w_node, projection.columns[3]);
}

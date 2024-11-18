#include "basis_yaml.h"
#include "yaml.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

BasisVariantConverter::BasisVariantConverter(YAML* yaml) :
        VariantConverter(yaml)
{
  vec_encoder = new Vector3VariantConverter(yaml);
  vec_encoder->set_format("flow");
}

BasisVariantConverter::~BasisVariantConverter()
{
  delete vec_encoder;
}

void BasisVariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  Basis basis = v.operator Basis();
  emit_as_map(node, basis);
}

Variant BasisVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (node.is_map() && node.has_child("x") && node.has_child("y") && node.has_child("z")) {
    Vector3 x = vec_encoder->decode(node["x"]).operator Vector3();
    Vector3 y = vec_encoder->decode(node["y"]).operator Vector3();
    Vector3 z = vec_encoder->decode(node["z"]).operator Vector3();
    return Basis(x, y, z);
  }
  throw YAMLException("invalid Basis format - " + String::utf8(node.val().str, node.val().len));
}

bool BasisVariantConverter::set_format(const String& format_str)
{
  return vec_encoder->set_format(format_str);
}

void BasisVariantConverter::emit_as_map(ryml::NodeRef& node, const Basis& basis) const
{
  node |= ryml::MAP;

  ryml::NodeRef x_node = node["x"];
  vec_encoder->encode(x_node, basis.get_column(0));

  ryml::NodeRef y_node = node["y"];
  vec_encoder->encode(y_node, basis.get_column(1));

  ryml::NodeRef z_node = node["z"];
  vec_encoder->encode(z_node, basis.get_column(2));
}

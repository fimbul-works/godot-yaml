#include "basis_yaml.h"
#include "../yaml_exception.h"

using namespace godot;

BasisVariantConverter::BasisVariantConverter(YAML* yaml) :
        VariantConverter(yaml)
{
  vec_encoder = new Vector3VariantConverter(yaml);
  vec_encoder->set_format("flow"); // Always use flow format for the Vector3 columns
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
  if (!node.is_map()) {
    throw YAMLException::create_invalid_format("Basis");
  }

  if (!node.has_child("x")) {
    throw YAMLException::create_missing_field("Basis", "x");
  }
  if (!node.has_child("y")) {
    throw YAMLException::create_missing_field("Basis", "y");
  }
  if (!node.has_child("z")) {
    throw YAMLException::create_missing_field("Basis", "z");
  }

  Vector3 x = vec_encoder->decode(node["x"]).operator Vector3();
  Vector3 y = vec_encoder->decode(node["y"]).operator Vector3();
  Vector3 z = vec_encoder->decode(node["z"]).operator Vector3();

  return Basis(x, y, z);
}

bool BasisVariantConverter::set_format(const String& format)
{
  // Basis only supports one format (map), but we delegate format setting to the Vector3 encoder
  return vec_encoder->set_format(format);
}

void BasisVariantConverter::emit_as_map(ryml::NodeRef& node, const Basis& basis) const
{
  node |= ryml::MAP;
  node |= ryml::FLOW_SL;

  ryml::NodeRef x_node = node["x"];
  vec_encoder->encode(x_node, basis.get_column(0));

  ryml::NodeRef y_node = node["y"];
  vec_encoder->encode(y_node, basis.get_column(1));

  ryml::NodeRef z_node = node["z"];
  vec_encoder->encode(z_node, basis.get_column(2));
}

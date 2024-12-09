#include "basis_yaml.h"
#include "../converter_factory.h"
#include "../exception.h"
#include "vector3_yaml.h"

using namespace godot;

BasisVariantConverter::BasisVariantConverter(ConverterFactory* factory) :
        vec3_converter(factory->create_converter_as<Vector3VariantConverter>(Variant::VECTOR3))
{
  ERR_FAIL_NULL(vec3_converter);
}

void BasisVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const YAMLStyle::View& style) const
{
  const Basis basis = v.operator Basis();

  if (!style.is_valid() || style.get_container_form() != YAMLStyle::FORM_SEQ) {
    emit_as_map(node, basis, style);
  } else {
    emit_as_sequence(node, basis, style);
  }
}

void BasisVariantConverter::emit_as_map(ryml::NodeRef& node, const Basis& basis, const YAMLStyle::View& style) const
{
  node |= ryml::MAP;
  style.apply_flow_style(node);

  // Pass child styles for each column
  YAMLStyle::View x_style = style.is_valid() ? style.get_child("x") : YAMLStyle::View();
  YAMLStyle::View y_style = style.is_valid() ? style.get_child("y") : YAMLStyle::View();
  YAMLStyle::View z_style = style.is_valid() ? style.get_child("z") : YAMLStyle::View();

  ryml::NodeRef x_node = node["x"];
  vec3_converter->encode(x_node, basis.get_column(0), x_style);

  ryml::NodeRef y_node = node["y"];
  vec3_converter->encode(y_node, basis.get_column(1), y_style);

  ryml::NodeRef z_node = node["z"];
  vec3_converter->encode(z_node, basis.get_column(2), z_style);
}

void BasisVariantConverter::emit_as_sequence(ryml::NodeRef& node, const Basis& basis, const YAMLStyle::View& style) const
{
  node |= ryml::SEQ;
  style.apply_flow_style(node);

  // Pass child styles using numeric indices
  YAMLStyle::View x_style = style.is_valid() ? style.get_child("0") : YAMLStyle::View();
  YAMLStyle::View y_style = style.is_valid() ? style.get_child("1") : YAMLStyle::View();
  YAMLStyle::View z_style = style.is_valid() ? style.get_child("2") : YAMLStyle::View();

  ryml::NodeRef x_node = node.append_child();
  vec3_converter->encode(x_node, basis.get_column(0), x_style);

  ryml::NodeRef y_node = node.append_child();
  vec3_converter->encode(y_node, basis.get_column(1), y_style);

  ryml::NodeRef z_node = node.append_child();
  vec3_converter->encode(z_node, basis.get_column(2), z_style);
}

Variant BasisVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  try {
    if (node.is_map()) {
      return decode_from_map(node);
    } else if (node.is_seq()) {
      return decode_from_sequence(node);
    }
    throw YAMLException::create_invalid_format("Basis");
  } catch (const YAMLException&) {
    throw; // Re-throw YAML exceptions
  } catch (const std::exception& e) {
    throw YAMLException::create_decode_error("Basis", e.what());
  }
}

Variant BasisVariantConverter::decode_from_map(const ryml::ConstNodeRef& node) const
{
  if (!node.has_child("x") || !node.has_child("y") || !node.has_child("z")) {
    throw YAMLException::create_missing_field("Basis", "x, y, z");
  }

  Vector3 x = vec3_converter->decode(node["x"]).operator Vector3();
  Vector3 y = vec3_converter->decode(node["y"]).operator Vector3();
  Vector3 z = vec3_converter->decode(node["z"]).operator Vector3();

  return Basis(x, y, z);
}

Variant BasisVariantConverter::decode_from_sequence(const ryml::ConstNodeRef& node) const
{
  if (node.num_children() != 3) {
    throw YAMLException::create_invalid_sequence_length("Basis", 3);
  }

  Vector3 x = vec3_converter->decode(node[0]).operator Vector3();
  Vector3 y = vec3_converter->decode(node[1]).operator Vector3();
  Vector3 z = vec3_converter->decode(node[2]).operator Vector3();

  return Basis(x, y, z);
}

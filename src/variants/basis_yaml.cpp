#include "basis_yaml.h"
#include "../exception.h"
#include "../variant_converter_registry.h"

using namespace godot;

void BasisVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const Ref<YAMLStyle>& style) const
{
  const Basis basis = v.operator Basis();

  if (!style.is_valid() || style->collection_style == YAMLStyle::COLLECTION_ANY
          || style->collection_style == YAMLStyle::MAP_BLOCK
          || style->collection_style == YAMLStyle::MAP_FLOW) {
    emit_as_map(node, basis, style);
  } else {
    emit_as_sequence(node, basis, style);
  }
}

void BasisVariantConverter::emit_as_map(ryml::NodeRef& node, const Basis& basis, const Ref<YAMLStyle>& style) const
{
  node |= ryml::MAP;
  if (style.is_valid() && style->collection_style == YAMLStyle::MAP_FLOW) {
    node |= ryml::FLOW_SL;
  }

  const auto* vec3_converter = VariantConverterRegistry::get_converter(Variant::VECTOR3);

  // Pass child styles for each column
  Ref<YAMLStyle> x_style = style.is_valid() ? style->get_child("x") : Ref<YAMLStyle>();
  Ref<YAMLStyle> y_style = style.is_valid() ? style->get_child("y") : Ref<YAMLStyle>();
  Ref<YAMLStyle> z_style = style.is_valid() ? style->get_child("z") : Ref<YAMLStyle>();

  ryml::NodeRef x_node = node["x"];
  vec3_converter->encode(x_node, basis.get_column(0), x_style);

  ryml::NodeRef y_node = node["y"];
  vec3_converter->encode(y_node, basis.get_column(1), y_style);

  ryml::NodeRef z_node = node["z"];
  vec3_converter->encode(z_node, basis.get_column(2), z_style);
}

void BasisVariantConverter::emit_as_sequence(ryml::NodeRef& node, const Basis& basis, const Ref<YAMLStyle>& style) const
{
  node |= ryml::SEQ;
  if (style.is_valid() && style->collection_style == YAMLStyle::COLLECTION_FLOW) {
    node |= ryml::FLOW_SL;
  }

  const auto* vec3_converter = VariantConverterRegistry::get_converter(Variant::VECTOR3);

  // Pass child styles using numeric indices
  Ref<YAMLStyle> x_style = style.is_valid() ? style->get_child("0") : Ref<YAMLStyle>();
  Ref<YAMLStyle> y_style = style.is_valid() ? style->get_child("1") : Ref<YAMLStyle>();
  Ref<YAMLStyle> z_style = style.is_valid() ? style->get_child("2") : Ref<YAMLStyle>();

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

  const auto* vec3_converter = VariantConverterRegistry::get_converter(Variant::VECTOR3);
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

  const auto* vec3_converter = VariantConverterRegistry::get_converter(Variant::VECTOR3);
  Vector3 x = vec3_converter->decode(node[0]).operator Vector3();
  Vector3 y = vec3_converter->decode(node[1]).operator Vector3();
  Vector3 z = vec3_converter->decode(node[2]).operator Vector3();

  return Basis(x, y, z);
}

#include "transform3d_yaml.h"
#include "../variant_converter_registry.h"
#include "../yaml_exception.h"

using namespace godot;

void Transform3DVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const
{
  const Transform3D transform = v.operator Transform3D();

  switch (format.get_format(Variant::TRANSFORM3D)) {
    case YAMLFormat::SEQUENCE:
      emit_as_sequence(node, transform, format);
      break;
    case YAMLFormat::BLOCK_MAP:
    case YAMLFormat::FLOW_MAP:
    default:
      emit_as_map(node, transform, format);
      break;
  }
}

void Transform3DVariantConverter::emit_as_map(ryml::NodeRef& node, const Transform3D& transform, const YAMLFormat::View& format) const
{
  node |= ryml::MAP;
  node |= ryml::FLOW_SL;

  const auto* basis_converter = VariantConverterRegistry::get_converter(Variant::BASIS);
  const auto* vec3_converter = VariantConverterRegistry::get_converter(Variant::VECTOR3);

  // Encode basis
  ryml::NodeRef basis_node = node["basis"];
  basis_converter->encode(basis_node, transform.basis, format);

  // Encode origin
  ryml::NodeRef origin_node = node["origin"];
  vec3_converter->encode(origin_node, transform.origin, format);
}

void Transform3DVariantConverter::emit_as_sequence(ryml::NodeRef& node, const Transform3D& transform, const YAMLFormat::View& format) const
{
  node |= ryml::SEQ;
  node |= ryml::FLOW_SL;

  const auto* vec3_converter = VariantConverterRegistry::get_converter(Variant::VECTOR3);

  // Encode basis columns
  for (int i = 0; i < 3; i++) {
    ryml::NodeRef col_node = node.append_child();
    vec3_converter->encode(col_node, transform.basis.rows[i], format);
  }

  // Encode origin
  ryml::NodeRef origin_node = node.append_child();
  vec3_converter->encode(origin_node, transform.origin, format);
}

Variant Transform3DVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  try {
    if (node.is_map()) {
      return decode_from_map(node);
    } else if (node.is_seq()) {
      return decode_from_sequence(node);
    }
    throw YAMLException::create_invalid_format("Transform3D");
  } catch (const YAMLException&) {
    throw;
  } catch (const std::exception& e) {
    throw YAMLException::create_decode_error("Transform3D", e.what());
  }
}

Variant Transform3DVariantConverter::decode_from_map(const ryml::ConstNodeRef& node) const
{
  if (!node.has_child("basis") || !node.has_child("origin")) {
    throw YAMLException::create_missing_field("Transform3D", "basis, origin");
  }

  const auto* basis_converter = VariantConverterRegistry::get_converter(Variant::BASIS);
  const auto* vec3_converter = VariantConverterRegistry::get_converter(Variant::VECTOR3);

  Basis basis = basis_converter->decode(node["basis"]).operator Basis();
  Vector3 origin = vec3_converter->decode(node["origin"]).operator Vector3();

  return Transform3D(basis, origin);
}

Variant Transform3DVariantConverter::decode_from_sequence(const ryml::ConstNodeRef& node) const
{
  if (node.num_children() != 4) {
    throw YAMLException::create_invalid_sequence_length("Transform3D", 4);
  }

  const auto* vec3_converter = VariantConverterRegistry::get_converter(Variant::VECTOR3);
  Basis basis;

  // Read basis columns
  for (int i = 0; i < 3; i++) {
    basis.rows[i] = vec3_converter->decode(node[i]).operator Vector3();
  }

  // Read origin
  Vector3 origin = vec3_converter->decode(node[3]).operator Vector3();

  return Transform3D(basis, origin);
}

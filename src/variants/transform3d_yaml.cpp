#include "transform3d_yaml.h"
#include "../exception.h"
#include "../variant_converter_registry.h"

using namespace godot;

void Transform3DVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const Ref<YAMLStyle>& style) const
{
  const Transform3D transform = v.operator Transform3D();

  if (!style.is_valid() || style->collection_style == YAMLStyle::COLLECTION_ANY
          || style->collection_style == YAMLStyle::MAP_BLOCK
          || style->collection_style == YAMLStyle::MAP_FLOW) {
    emit_as_map(node, transform, style);
  } else {
    emit_as_sequence(node, transform, style);
  }
}

void Transform3DVariantConverter::emit_as_map(ryml::NodeRef& node, const Transform3D& transform, const Ref<YAMLStyle>& style) const
{
  node |= ryml::MAP;
  if (style.is_valid() && style->collection_style == YAMLStyle::MAP_FLOW) {
    node |= ryml::FLOW_SL;
  }

  const auto* basis_converter = VariantConverterRegistry::get_converter(Variant::BASIS);
  const auto* vec3_converter = VariantConverterRegistry::get_converter(Variant::VECTOR3);

  // Pass child styles for basis and origin
  Ref<YAMLStyle> basis_style = style.is_valid() ? style->get_child("basis") : Ref<YAMLStyle>();
  Ref<YAMLStyle> origin_style = style.is_valid() ? style->get_child("origin") : Ref<YAMLStyle>();

  ryml::NodeRef basis_node = node["basis"];
  basis_converter->encode(basis_node, transform.basis, basis_style);

  ryml::NodeRef origin_node = node["origin"];
  vec3_converter->encode(origin_node, transform.origin, origin_style);
}

void Transform3DVariantConverter::emit_as_sequence(ryml::NodeRef& node, const Transform3D& transform, const Ref<YAMLStyle>& style) const
{
  node |= ryml::SEQ;
  if (style.is_valid() && style->collection_style == YAMLStyle::COLLECTION_FLOW) {
    node |= ryml::FLOW_SL;
  }

  const auto* vec3_converter = VariantConverterRegistry::get_converter(Variant::VECTOR3);

  // Pass child styles for each vector using indices
  for (int i = 0; i < 3; i++) {
    Ref<YAMLStyle> row_style = style.is_valid() ? style->get_child(String::num_int64(i)) : Ref<YAMLStyle>();
    ryml::NodeRef col_node = node.append_child();
    vec3_converter->encode(col_node, transform.basis.rows[i], row_style);
  }

  // Origin gets index 3
  Ref<YAMLStyle> origin_style = style.is_valid() ? style->get_child("3") : Ref<YAMLStyle>();
  ryml::NodeRef origin_node = node.append_child();
  vec3_converter->encode(origin_node, transform.origin, origin_style);
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

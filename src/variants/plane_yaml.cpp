#include "plane_yaml.h"
#include "../exception.h"
#include "../util_numeric.h"
#include "../variant_converter_registry.h"

using namespace godot;

void PlaneVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const Ref<YAMLStyle>& style) const
{
  const Plane plane = v.operator Plane();

  if (!style.is_valid() || style->collection_style == YAMLStyle::COLLECTION_ANY
          || style->collection_style == YAMLStyle::MAP_BLOCK
          || style->collection_style == YAMLStyle::MAP_FLOW) {
    emit_as_map(node, plane, style);
  } else {
    emit_as_sequence(node, plane, style);
  }
}

void PlaneVariantConverter::emit_as_map(ryml::NodeRef& node, const Plane& plane, const Ref<YAMLStyle>& style) const
{
  node |= ryml::MAP;
  if (style.is_valid() && style->collection_style == YAMLStyle::MAP_FLOW) {
    node |= ryml::FLOW_SL;
  }

  const auto* vec3_converter = VariantConverterRegistry::get_converter(Variant::VECTOR3);

  // Pass child styles for nested components
  Ref<YAMLStyle> normal_style = style.is_valid() ? style->get_child("normal") : Ref<YAMLStyle>();
  ryml::NodeRef normal_node = node["normal"];
  vec3_converter->encode(normal_node, plane.normal, normal_style);

  // The d component gets its own style
  Ref<YAMLStyle> d_style = style.is_valid() ? style->get_child("d") : Ref<YAMLStyle>();
  if (d_style.is_valid()) {
    node["d"] << float_to_string(plane.d, d_style->number_format);
  } else {
    node["d"] << float_to_string(plane.d);
  }
}

void PlaneVariantConverter::emit_as_sequence(ryml::NodeRef& node, const Plane& plane, const Ref<YAMLStyle>& style) const
{
  node |= ryml::SEQ;
  if (style.is_valid() && style->collection_style == YAMLStyle::COLLECTION_FLOW) {
    node |= ryml::FLOW_SL;
  }

  const auto* vec3_converter = VariantConverterRegistry::get_converter(Variant::VECTOR3);

  // Pass child styles for nested components using numeric indices
  Ref<YAMLStyle> normal_style = style.is_valid() ? style->get_child("0") : Ref<YAMLStyle>();
  ryml::NodeRef normal_node = node.append_child();
  vec3_converter->encode(normal_node, plane.normal, normal_style);

  // Style for d component
  Ref<YAMLStyle> d_style = style.is_valid() ? style->get_child("1") : Ref<YAMLStyle>();
  if (d_style.is_valid()) {
    node.append_child() << float_to_string(plane.d, d_style->number_format);
  } else {
    node.append_child() << float_to_string(plane.d);
  }
}

Variant PlaneVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  try {
    if (node.is_map()) {
      return decode_from_map(node);
    } else if (node.is_seq()) {
      return decode_from_sequence(node);
    }
    throw YAMLException::create_invalid_format("Plane");
  } catch (const YAMLException&) {
    throw;
  } catch (const std::exception& e) {
    throw YAMLException::create_decode_error("Plane", e.what());
  }
}

Variant PlaneVariantConverter::decode_from_map(const ryml::ConstNodeRef& node) const
{
  if (!node.has_child("normal") || !node.has_child("d")) {
    throw YAMLException::create_missing_field("Plane", "normal, d");
  }

  const auto* vec3_converter = VariantConverterRegistry::get_converter(Variant::VECTOR3);
  Vector3 normal = vec3_converter->decode(node["normal"]).operator Vector3();
  real_t d = string_to_float<real_t>(node["d"].val());

  return Plane(normal, d);
}

Variant PlaneVariantConverter::decode_from_sequence(const ryml::ConstNodeRef& node) const
{
  if (node.num_children() != 2) {
    throw YAMLException::create_invalid_sequence_length("Plane", 2);
  }

  const auto* vec3_converter = VariantConverterRegistry::get_converter(Variant::VECTOR3);
  Vector3 normal = vec3_converter->decode(node[0]).operator Vector3();
  real_t d = string_to_float<real_t>(node[1].val());

  return Plane(normal, d);
}

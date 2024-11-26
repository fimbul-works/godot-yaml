#include "plane_yaml.h"
#include "../util_numeric.h"
#include "../variant_converter_registry.h"
#include "../yaml_exception.h"

using namespace godot;

void PlaneVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const
{
  const Plane plane = v.operator Plane();

  // Check format and use appropriate encoding method
  switch (format.get_format(Variant::PLANE)) {
    case YAMLFormat::SEQUENCE:
      emit_as_sequence(node, plane, format);
      break;
    case YAMLFormat::BLOCK_MAP:
    case YAMLFormat::FLOW_MAP:
    default:
      emit_as_map(node, plane, format);
      break;
  }
}

void PlaneVariantConverter::emit_as_map(ryml::NodeRef& node, const Plane& plane, const YAMLFormat::View& format) const
{
  node |= ryml::MAP;
  node |= ryml::FLOW_SL;

  const auto* vec3_converter = get_vec3_converter();

  ryml::NodeRef normal_node = node["normal"];
  vec3_converter->encode(normal_node, plane.normal, format);

  node["d"] << float_to_string(plane.d);
}

void PlaneVariantConverter::emit_as_sequence(ryml::NodeRef& node, const Plane& plane, const YAMLFormat::View& format) const
{
  node |= ryml::SEQ;
  node |= ryml::FLOW_SL;

  const auto* vec3_converter = get_vec3_converter();

  ryml::NodeRef normal_node = node.append_child();
  vec3_converter->encode(normal_node, plane.normal, format);

  node.append_child() << float_to_string(plane.d);
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
    throw YAMLException(String("Failed to decode Plane: ") + e.what());
  }
}

Variant PlaneVariantConverter::decode_from_map(const ryml::ConstNodeRef& node) const
{
  if (!node.has_child("normal") || !node.has_child("d")) {
    throw YAMLException::create_missing_field("Plane", "normal, d");
  }

  const auto* vec3_converter = get_vec3_converter();
  Vector3 normal = vec3_converter->decode(node["normal"]).operator Vector3();
  real_t d = string_to_float<real_t>(node["d"].val());

  return Plane(normal, d);
}

Variant PlaneVariantConverter::decode_from_sequence(const ryml::ConstNodeRef& node) const
{
  if (node.num_children() != 2) {
    throw YAMLException::create_invalid_sequence_length("Plane", 2);
  }

  const auto* vec3_converter = get_vec3_converter();
  Vector3 normal = vec3_converter->decode(node[0]).operator Vector3();
  real_t d = string_to_float<real_t>(node[1].val());

  return Plane(normal, d);
}

const VariantConverter* PlaneVariantConverter::get_vec3_converter() const
{
  const auto* converter = VariantConverterRegistry::get_converter(Variant::VECTOR3);
  if (!converter) {
    throw YAMLException("Vector3 converter not found in registry");
  }
  return converter;
}

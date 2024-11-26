#include "quaternion_yaml.h"
#include "../util_numeric.h"
#include "../variant_converter_registry.h"
#include "../yaml_exception.h"

using namespace godot;

void QuaternionVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const
{
  const Quaternion quat = v.operator Quaternion();

  switch (format.get_format(Variant::QUATERNION)) {
    case YAMLFormat::SEQUENCE:
      emit_as_sequence(node, quat, format);
      break;
    case YAMLFormat::CUSTOM_1: // Axis-Angle format
      emit_as_axis_angle(node, quat, format);
      break;
    case YAMLFormat::BLOCK_MAP:
    case YAMLFormat::FLOW_MAP:
    default:
      emit_as_map(node, quat, format);
      break;
  }
}

void QuaternionVariantConverter::emit_as_map(ryml::NodeRef& node, const Quaternion& quat, const YAMLFormat::View& format) const
{
  node |= ryml::MAP;
  node |= ryml::FLOW_SL;

  node["x"] << float_to_string(quat.x);
  node["y"] << float_to_string(quat.y);
  node["z"] << float_to_string(quat.z);
  node["w"] << float_to_string(quat.w);
}

void QuaternionVariantConverter::emit_as_sequence(ryml::NodeRef& node, const Quaternion& quat, const YAMLFormat::View& format) const
{
  node |= ryml::SEQ;
  node |= ryml::FLOW_SL;

  node.append_child() << float_to_string(quat.x);
  node.append_child() << float_to_string(quat.y);
  node.append_child() << float_to_string(quat.z);
  node.append_child() << float_to_string(quat.w);
}

void QuaternionVariantConverter::emit_as_axis_angle(ryml::NodeRef& node, const Quaternion& quat, const YAMLFormat::View& format) const
{
  node |= ryml::MAP;
  node |= ryml::FLOW_SL;

  Vector3 axis;
  real_t angle;
  quat.get_axis_angle(axis, angle);

  const auto* vec3_converter = get_vec3_converter();
  ryml::NodeRef axis_node = node["axis"];
  vec3_converter->encode(axis_node, axis, format);

  node["angle"] << float_to_string(angle);
}

Variant QuaternionVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  try {
    if (node.is_map()) {
      // Check for axis-angle format first
      if (node.has_child("axis") && node.has_child("angle")) {
        return decode_from_axis_angle(node);
      }
      // Default to component map
      return decode_from_map(node);
    } else if (node.is_seq()) {
      return decode_from_sequence(node);
    }
    throw YAMLException::create_invalid_format("Quaternion");
  } catch (const YAMLException&) {
    throw;
  } catch (const std::exception& e) {
    throw YAMLException(String("Failed to decode Quaternion: ") + e.what());
  }
}

Variant QuaternionVariantConverter::decode_from_map(const ryml::ConstNodeRef& node) const
{
  if (!node.has_child("x") || !node.has_child("y") || !node.has_child("z") || !node.has_child("w")) {
    throw YAMLException::create_missing_field("Quaternion", "x, y, z, w");
  }

  return Quaternion(
          string_to_float<real_t>(node["x"].val()),
          string_to_float<real_t>(node["y"].val()),
          string_to_float<real_t>(node["z"].val()),
          string_to_float<real_t>(node["w"].val()));
}

Variant QuaternionVariantConverter::decode_from_sequence(const ryml::ConstNodeRef& node) const
{
  if (node.num_children() != 4) {
    throw YAMLException::create_invalid_sequence_length("Quaternion", 4);
  }

  return Quaternion(
          string_to_float<real_t>(node[0].val()),
          string_to_float<real_t>(node[1].val()),
          string_to_float<real_t>(node[2].val()),
          string_to_float<real_t>(node[3].val()));
}

Variant QuaternionVariantConverter::decode_from_axis_angle(const ryml::ConstNodeRef& node) const
{
  if (!node.has_child("axis") || !node.has_child("angle")) {
    throw YAMLException::create_missing_field("Quaternion", "axis, angle");
  }

  const auto* vec3_converter = get_vec3_converter();
  Vector3 axis = vec3_converter->decode(node["axis"]).operator Vector3();
  real_t angle = string_to_float<real_t>(node["angle"].val());

  return Quaternion(axis, angle);
}

const VariantConverter* QuaternionVariantConverter::get_vec3_converter() const
{
  const auto* converter = VariantConverterRegistry::get_converter(Variant::VECTOR3);
  if (!converter) {
    throw YAMLException("Vector3 converter not found in registry");
  }
  return converter;
}

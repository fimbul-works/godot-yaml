#include "vector3i_yaml.h"
#include "../util_numeric.h"
#include "../yaml_exception.h"

using namespace godot;

void Vector3iVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const
{
  const Vector3i vec = v.operator Vector3i();

  switch (format.get_format(Variant::VECTOR3I)) {
    case YAMLFormat::SEQUENCE:
      emit_as_sequence(node, vec);
      break;
    case YAMLFormat::BLOCK_MAP:
    case YAMLFormat::FLOW_MAP:
    default:
      emit_as_map(node, vec);
      break;
  }
}

void Vector3iVariantConverter::emit_as_map(ryml::NodeRef& node, const Vector3i& vec) const
{
  node |= ryml::MAP;
  node |= ryml::FLOW_SL;
  node["x"] << int_to_string(vec.x);
  node["y"] << int_to_string(vec.y);
  node["z"] << int_to_string(vec.z);
}

void Vector3iVariantConverter::emit_as_sequence(ryml::NodeRef& node, const Vector3i& vec) const
{
  node |= ryml::SEQ;
  node |= ryml::FLOW_SL;
  node.append_child() << int_to_string(vec.x);
  node.append_child() << int_to_string(vec.y);
  node.append_child() << int_to_string(vec.z);
}

Variant Vector3iVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  try {
    if (node.is_map()) {
      return decode_from_map(node);
    } else if (node.is_seq()) {
      return decode_from_sequence(node);
    }
    throw YAMLException::create_invalid_format("Vector3i");
  } catch (const YAMLException&) {
    throw;
  } catch (const std::exception& e) {
    throw YAMLException::create_decode_error("Vector3i", e.what());
  }
}

Variant Vector3iVariantConverter::decode_from_map(const ryml::ConstNodeRef& node) const
{
  if (!node.has_child("x") || !node.has_child("y") || !node.has_child("z")) {
    throw YAMLException::create_missing_field("Vector3i", "x, y, z");
  }

  return Vector3i(
          string_to_int<int32_t>(node["x"].val()),
          string_to_int<int32_t>(node["y"].val()),
          string_to_int<int32_t>(node["z"].val()));
}

Variant Vector3iVariantConverter::decode_from_sequence(const ryml::ConstNodeRef& node) const
{
  if (node.num_children() != 3) {
    throw YAMLException::create_invalid_sequence_length("Vector3i", 3);
  }

  return Vector3i(
          string_to_int<int32_t>(node[0].val()),
          string_to_int<int32_t>(node[1].val()),
          string_to_int<int32_t>(node[2].val()));
}

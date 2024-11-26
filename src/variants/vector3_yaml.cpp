#include "vector3_yaml.h"
#include "../exception.h"
#include "../util_numeric.h"

using namespace godot;

void Vector3VariantConverter::encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const
{
  const Vector3 vec = v.operator Vector3();

  switch (format.get_format(Variant::VECTOR3)) {
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

void Vector3VariantConverter::emit_as_map(ryml::NodeRef& node, const Vector3& vec) const
{
  node |= ryml::MAP;
  node |= ryml::FLOW_SL;
  node["x"] << float_to_string(vec.x);
  node["y"] << float_to_string(vec.y);
  node["z"] << float_to_string(vec.z);
}

void Vector3VariantConverter::emit_as_sequence(ryml::NodeRef& node, const Vector3& vec) const
{
  node |= ryml::SEQ;
  node |= ryml::FLOW_SL;
  node.append_child() << float_to_string(vec.x);
  node.append_child() << float_to_string(vec.y);
  node.append_child() << float_to_string(vec.z);
}

Variant Vector3VariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  try {
    if (node.is_map()) {
      return decode_from_map(node);
    } else if (node.is_seq()) {
      return decode_from_sequence(node);
    }
    throw YAMLException::create_invalid_format("Vector3");
  } catch (const YAMLException&) {
    throw;
  } catch (const std::exception& e) {
    throw YAMLException::create_decode_error("Vector3", e.what());
  }
}

Variant Vector3VariantConverter::decode_from_map(const ryml::ConstNodeRef& node) const
{
  if (!node.has_child("x") || !node.has_child("y") || !node.has_child("z")) {
    throw YAMLException::create_missing_field("Vector3", "x, y, z");
  }

  return Vector3(
          string_to_float<real_t>(node["x"].val()),
          string_to_float<real_t>(node["y"].val()),
          string_to_float<real_t>(node["z"].val()));
}

Variant Vector3VariantConverter::decode_from_sequence(const ryml::ConstNodeRef& node) const
{
  if (node.num_children() != 3) {
    throw YAMLException::create_invalid_sequence_length("Vector3", 3);
  }

  return Vector3(
          string_to_float<real_t>(node[0].val()),
          string_to_float<real_t>(node[1].val()),
          string_to_float<real_t>(node[2].val()));
}

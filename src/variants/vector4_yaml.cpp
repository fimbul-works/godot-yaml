#include "vector4_yaml.h"
#include "../util_numeric.h"
#include "../yaml_exception.h"

using namespace godot;

void Vector4VariantConverter::encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const
{
  const Vector4 vec = v.operator Vector4();

  switch (format.get_format(Variant::VECTOR4)) {
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

void Vector4VariantConverter::emit_as_map(ryml::NodeRef& node, const Vector4& vec) const
{
  node |= ryml::MAP;
  node |= ryml::FLOW_SL;
  node["x"] << float_to_string(vec.x);
  node["y"] << float_to_string(vec.y);
  node["z"] << float_to_string(vec.z);
  node["w"] << float_to_string(vec.w);
}

void Vector4VariantConverter::emit_as_sequence(ryml::NodeRef& node, const Vector4& vec) const
{
  node |= ryml::SEQ;
  node |= ryml::FLOW_SL;
  node.append_child() << float_to_string(vec.x);
  node.append_child() << float_to_string(vec.y);
  node.append_child() << float_to_string(vec.z);
  node.append_child() << float_to_string(vec.w);
}

Variant Vector4VariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  try {
    if (node.is_map()) {
      return decode_from_map(node);
    } else if (node.is_seq()) {
      return decode_from_sequence(node);
    }
    throw YAMLException::create_invalid_format("Vector4");
  } catch (const YAMLException&) {
    throw;
  } catch (const std::exception& e) {
    throw YAMLException::create_decode_error("Vector4", e.what());
  }
}

Variant Vector4VariantConverter::decode_from_map(const ryml::ConstNodeRef& node) const
{
  if (!node.has_child("x") || !node.has_child("y") || !node.has_child("z") || !node.has_child("w")) {
    throw YAMLException::create_missing_field("Vector4", "x, y, z, w");
  }

  return Vector4(
          string_to_float<real_t>(node["x"].val()),
          string_to_float<real_t>(node["y"].val()),
          string_to_float<real_t>(node["z"].val()),
          string_to_float<real_t>(node["w"].val()));
}

Variant Vector4VariantConverter::decode_from_sequence(const ryml::ConstNodeRef& node) const
{
  if (node.num_children() != 4) {
    throw YAMLException::create_invalid_sequence_length("Vector4", 4);
  }

  return Vector4(
          string_to_float<real_t>(node[0].val()),
          string_to_float<real_t>(node[1].val()),
          string_to_float<real_t>(node[2].val()),
          string_to_float<real_t>(node[3].val()));
}

#include "vector2i_yaml.h"
#include "../util_numeric.h"
#include "../yaml_exception.h"

using namespace godot;

void Vector2iVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const
{
  const Vector2i vec = v.operator Vector2i();

  switch (format.get_format(Variant::VECTOR2I)) {
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

void Vector2iVariantConverter::emit_as_map(ryml::NodeRef& node, const Vector2i& vec) const
{
  node |= ryml::MAP;
  node |= ryml::FLOW_SL;
  node["x"] << int_to_string(vec.x);
  node["y"] << int_to_string(vec.y);
}

void Vector2iVariantConverter::emit_as_sequence(ryml::NodeRef& node, const Vector2i& vec) const
{
  node |= ryml::SEQ;
  node |= ryml::FLOW_SL;
  node.append_child() << int_to_string(vec.x);
  node.append_child() << int_to_string(vec.y);
}

Variant Vector2iVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  try {
    if (node.is_map()) {
      return decode_from_map(node);
    } else if (node.is_seq()) {
      return decode_from_sequence(node);
    }
    throw YAMLException::create_invalid_format("Vector2i");
  } catch (const YAMLException&) {
    throw;
  } catch (const std::exception& e) {
    throw YAMLException(String("Failed to decode Vector2i: ") + e.what());
  }
}

Variant Vector2iVariantConverter::decode_from_map(const ryml::ConstNodeRef& node) const
{
  if (!node.has_child("x") || !node.has_child("y")) {
    throw YAMLException::create_missing_field("Vector2i", "x, y");
  }

  return Vector2i(
          string_to_int<int32_t>(node["x"].val()),
          string_to_int<int32_t>(node["y"].val()));
}

Variant Vector2iVariantConverter::decode_from_sequence(const ryml::ConstNodeRef& node) const
{
  if (node.num_children() != 2) {
    throw YAMLException::create_invalid_sequence_length("Vector2i", 2);
  }

  return Vector2i(
          string_to_int<int32_t>(node[0].val()),
          string_to_int<int32_t>(node[1].val()));
}

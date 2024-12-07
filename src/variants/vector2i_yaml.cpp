#include "vector2i_yaml.h"
#include "../exception.h"
#include "../util_numeric.h"

using namespace godot;

void Vector2iVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const
{
  const Vector2i vec = v.operator Vector2i();
  YAMLFormat::Format fmt = format.get_format(Variant::VECTOR2I);

  if (fmt == YAMLFormat::SEQUENCE || fmt == YAMLFormat::FLOW_SEQUENCE) {
    emit_as_sequence(node, vec, format);
  } else {
    emit_as_map(node, vec, format);
  }
}

void Vector2iVariantConverter::emit_as_map(ryml::NodeRef& node, const Vector2i& vec, const YAMLFormat::View& format) const
{
  node |= ryml::MAP;

  if (format.get_format(Variant::VECTOR2I) == YAMLFormat::FLOW_MAP) {
    node |= ryml::FLOW_SL;
  }

  node["x"] << int_to_string(vec.x);
  node["y"] << int_to_string(vec.y);
}

void Vector2iVariantConverter::emit_as_sequence(ryml::NodeRef& node, const Vector2i& vec, const YAMLFormat::View& format) const
{
  node |= ryml::SEQ;

  if (format.get_format(Variant::VECTOR2I) == YAMLFormat::FLOW_SEQUENCE) {
    node |= ryml::FLOW_SL;
  }

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
    throw YAMLException::create_decode_error("Vector2i", e.what());
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

#include "vector2i_yaml.h"
#include "../exception.h"
#include "../util_numeric.h"

using namespace godot;

void Vector2iVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const YAMLStyle::View& style) const
{
  const Vector2i vec = v.operator Vector2i();

  if (!style.is_valid() || style.get_container_form() != YAMLStyle::FORM_SEQ) {
    // Map styles
    node |= ryml::MAP;

    // Flow style
    if (!style.is_valid()) {
      node |= ryml::FLOW_SL;
    } else {
      style.apply_flow_style(node);
    }

    node["x"] << int_to_string(vec.x);
    node["y"] << int_to_string(vec.y);
  } else {
    // Collection styles
    node |= ryml::SEQ;

    // Flow style
    if (!style.is_valid()) {
      node |= ryml::FLOW_SL;
    } else {
      style.apply_flow_style(node);
    }

    node.append_child() << int_to_string(vec.x);
    node.append_child() << int_to_string(vec.y);
  }
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

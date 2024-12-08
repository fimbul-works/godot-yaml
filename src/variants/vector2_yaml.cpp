#include "vector2_yaml.h"
#include "../exception.h"
#include "../util_numeric.h"

using namespace godot;

void Vector2VariantConverter::encode(ryml::NodeRef& node, const Variant& v, const Ref<YAMLStyle>& style) const
{
  const Vector2 vec = v.operator Vector2();

  if (!style.is_valid() || style->collection_style == YAMLStyle::COLLECTION_ANY
          || style->collection_style == YAMLStyle::MAP_BLOCK
          || style->collection_style == YAMLStyle::MAP_FLOW) {
    // Map styles
    node |= ryml::MAP;
    if (!style.is_valid() || style->collection_style == YAMLStyle::MAP_FLOW) {
      node |= ryml::FLOW_SL;
    }

    node["x"] << float_to_string(vec.x);
    node["y"] << float_to_string(vec.y);
  } else {
    // Collection styles
    node |= ryml::SEQ;
    if (!style.is_valid() || style->collection_style == YAMLStyle::COLLECTION_FLOW) {
      node |= ryml::FLOW_SL;
    }

    node.append_child() << float_to_string(vec.x);
    node.append_child() << float_to_string(vec.y);
  }
}

Variant Vector2VariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  try {
    if (node.is_map()) {
      return decode_from_map(node);
    } else if (node.is_seq()) {
      return decode_from_sequence(node);
    }
    throw YAMLException::create_invalid_format("Vector2");
  } catch (const YAMLException&) {
    throw;
  } catch (const std::exception& e) {
    throw YAMLException::create_decode_error("Vector2", e.what());
  }
}

Variant Vector2VariantConverter::decode_from_map(const ryml::ConstNodeRef& node) const
{
  if (!node.has_child("x") || !node.has_child("y")) {
    throw YAMLException::create_missing_field("Vector2", "x, y");
  }

  return Vector2(
          string_to_float<real_t>(node["x"].val()),
          string_to_float<real_t>(node["y"].val()));
}

Variant Vector2VariantConverter::decode_from_sequence(const ryml::ConstNodeRef& node) const
{
  if (node.num_children() != 2) {
    throw YAMLException::create_invalid_sequence_length("Vector2", 2);
  }

  return Vector2(
          string_to_float<real_t>(node[0].val()),
          string_to_float<real_t>(node[1].val()));
}

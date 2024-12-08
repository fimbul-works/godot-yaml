#include "vector4i_yaml.h"
#include "../exception.h"
#include "../util_numeric.h"

using namespace godot;

void Vector4iVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const Ref<YAMLStyle>& style) const
{
  const Vector4i vec = v.operator Vector4i();

  if (!style.is_valid() || style->collection_style == YAMLStyle::COLLECTION_ANY
          || style->collection_style == YAMLStyle::MAP_BLOCK
          || style->collection_style == YAMLStyle::MAP_FLOW) {
    // Map styles
    node |= ryml::MAP;
    if (!style.is_valid() || style->collection_style == YAMLStyle::MAP_FLOW) {
      node |= ryml::FLOW_SL;
    }

    node["x"] << int_to_string(vec.x);
    node["y"] << int_to_string(vec.y);
    node["z"] << int_to_string(vec.z);
    node["w"] << int_to_string(vec.w);
  } else {
    // Collection styles
    node |= ryml::SEQ;
    if (!style.is_valid() || style->collection_style == YAMLStyle::COLLECTION_FLOW) {
      node |= ryml::FLOW_SL;
    }

    node.append_child() << int_to_string(vec.x);
    node.append_child() << int_to_string(vec.y);
    node.append_child() << int_to_string(vec.z);
    node.append_child() << int_to_string(vec.w);
  }
}

Variant Vector4iVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  try {
    if (node.is_map()) {
      return decode_from_map(node);
    } else if (node.is_seq()) {
      return decode_from_sequence(node);
    }
    throw YAMLException::create_invalid_format("Vector4i");
  } catch (const YAMLException&) {
    throw;
  } catch (const std::exception& e) {
    throw YAMLException::create_decode_error("Vector4i", e.what());
  }
}

Variant Vector4iVariantConverter::decode_from_map(const ryml::ConstNodeRef& node) const
{
  if (!node.has_child("x") || !node.has_child("y") || !node.has_child("z") || !node.has_child("w")) {
    throw YAMLException::create_missing_field("Vector4i", "x, y, z, w");
  }

  return Vector4i(
          string_to_int<int32_t>(node["x"].val()),
          string_to_int<int32_t>(node["y"].val()),
          string_to_int<int32_t>(node["z"].val()),
          string_to_int<int32_t>(node["w"].val()));
}

Variant Vector4iVariantConverter::decode_from_sequence(const ryml::ConstNodeRef& node) const
{
  if (node.num_children() != 4) {
    throw YAMLException::create_invalid_sequence_length("Vector4i", 4);
  }

  return Vector4i(
          string_to_int<int32_t>(node[0].val()),
          string_to_int<int32_t>(node[1].val()),
          string_to_int<int32_t>(node[2].val()),
          string_to_int<int32_t>(node[3].val()));
}

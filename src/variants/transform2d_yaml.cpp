#include "transform2d_yaml.h"
#include "../exception.h"
#include "../variant_converter_registry.h"

using namespace godot;

void Transform2DVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const
{
  const Transform2D transform = v.operator Transform2D();
  YAMLFormat::Format fmt = format.get_format(Variant::TRANSFORM2D);

  if (fmt == YAMLFormat::SEQUENCE || fmt == YAMLFormat::FLOW_SEQUENCE) {
    emit_as_sequence(node, transform, format);
  } else {
    emit_as_map(node, transform, format);
  }
}

void Transform2DVariantConverter::emit_as_map(ryml::NodeRef& node, const Transform2D& transform, const YAMLFormat::View& format) const
{
  node |= ryml::MAP;

  if (format.get_format(Variant::TRANSFORM2D) == YAMLFormat::FLOW_MAP) {
    node |= ryml::FLOW_SL;
  }

  const auto* vec2_converter = VariantConverterRegistry::get_converter(Variant::VECTOR2);

  // Encode the matrix columns (x, y components)
  ryml::NodeRef x_node = node["x"];
  vec2_converter->encode(x_node, transform.columns[0], format);

  ryml::NodeRef y_node = node["y"];
  vec2_converter->encode(y_node, transform.columns[1], format);

  // Encode the origin/translation component
  ryml::NodeRef origin_node = node["origin"];
  vec2_converter->encode(origin_node, transform.columns[2], format);
}

void Transform2DVariantConverter::emit_as_sequence(ryml::NodeRef& node, const Transform2D& transform, const YAMLFormat::View& format) const
{
  node |= ryml::SEQ;

  if (format.get_format(Variant::TRANSFORM2D) == YAMLFormat::FLOW_SEQUENCE) {
    node |= ryml::FLOW_SL;
  }

  const auto* vec2_converter = VariantConverterRegistry::get_converter(Variant::VECTOR2);

  // Encode columns in order: x, y, origin
  ryml::NodeRef x_node = node.append_child();
  vec2_converter->encode(x_node, transform.columns[0], format);

  ryml::NodeRef y_node = node.append_child();
  vec2_converter->encode(y_node, transform.columns[1], format);

  ryml::NodeRef origin_node = node.append_child();
  vec2_converter->encode(origin_node, transform.columns[2], format);
}

Variant Transform2DVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  try {
    if (node.is_map()) {
      return decode_from_map(node);
    } else if (node.is_seq()) {
      return decode_from_sequence(node);
    }
    throw YAMLException::create_invalid_format("Transform2D");
  } catch (const YAMLException&) {
    throw;
  } catch (const std::exception& e) {
    throw YAMLException::create_decode_error("Transform2D", e.what());
  }
}

Variant Transform2DVariantConverter::decode_from_map(const ryml::ConstNodeRef& node) const
{
  if (!node.has_child("x") || !node.has_child("y") || !node.has_child("origin")) {
    throw YAMLException::create_missing_field("Transform2D", "x, y, origin");
  }

  const auto* vec2_converter = VariantConverterRegistry::get_converter(Variant::VECTOR2);
  Vector2 x = vec2_converter->decode(node["x"]).operator Vector2();
  Vector2 y = vec2_converter->decode(node["y"]).operator Vector2();
  Vector2 origin = vec2_converter->decode(node["origin"]).operator Vector2();

  return Transform2D(x, y, origin);
}

Variant Transform2DVariantConverter::decode_from_sequence(const ryml::ConstNodeRef& node) const
{
  if (node.num_children() != 3) {
    throw YAMLException::create_invalid_sequence_length("Transform2D", 3);
  }

  const auto* vec2_converter = VariantConverterRegistry::get_converter(Variant::VECTOR2);
  Vector2 x = vec2_converter->decode(node[0]).operator Vector2();
  Vector2 y = vec2_converter->decode(node[1]).operator Vector2();
  Vector2 origin = vec2_converter->decode(node[2]).operator Vector2();

  return Transform2D(x, y, origin);
}

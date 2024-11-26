#include "rect2_yaml.h"
#include "../exception.h"
#include "../util_numeric.h"
#include "../variant_converter_registry.h"

using namespace godot;

void Rect2VariantConverter::encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const
{
  const Rect2 rect = v.operator Rect2();

  switch (format.get_format(Variant::RECT2)) {
    case YAMLFormat::SEQUENCE:
      emit_as_sequence(node, rect, format);
      break;
    case YAMLFormat::CUSTOM_1: // Expanded format
      emit_as_expanded(node, rect);
      break;
    case YAMLFormat::BLOCK_MAP:
    case YAMLFormat::FLOW_MAP:
    default:
      emit_as_map(node, rect, format);
      break;
  }
}

void Rect2VariantConverter::emit_as_map(ryml::NodeRef& node, const Rect2& rect, const YAMLFormat::View& format) const
{
  node |= ryml::MAP;
  node |= ryml::FLOW_SL;

  const auto* vec2_converter = VariantConverterRegistry::get_converter(Variant::VECTOR2);

  ryml::NodeRef pos_node = node["position"];
  vec2_converter->encode(pos_node, rect.position, format);

  ryml::NodeRef size_node = node["size"];
  vec2_converter->encode(size_node, rect.size, format);
}

void Rect2VariantConverter::emit_as_sequence(ryml::NodeRef& node, const Rect2& rect, const YAMLFormat::View& format) const
{
  node |= ryml::SEQ;
  node |= ryml::FLOW_SL;

  const auto* vec2_converter = VariantConverterRegistry::get_converter(Variant::VECTOR2);

  ryml::NodeRef pos_node = node.append_child();
  vec2_converter->encode(pos_node, rect.position, format);

  ryml::NodeRef size_node = node.append_child();
  vec2_converter->encode(size_node, rect.size, format);
}

void Rect2VariantConverter::emit_as_expanded(ryml::NodeRef& node, const Rect2& rect) const
{
  node |= ryml::MAP;
  node |= ryml::FLOW_SL;

  node["x"] << float_to_string(rect.position.x);
  node["y"] << float_to_string(rect.position.y);
  node["w"] << float_to_string(rect.size.x);
  node["h"] << float_to_string(rect.size.y);
}

Variant Rect2VariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  try {
    if (node.is_map()) {
      // Check for expanded format first
      if (node.has_child("x") && node.has_child("y") && node.has_child("w") && node.has_child("h")) {
        return decode_from_expanded(node);
      }
      return decode_from_map(node);
    } else if (node.is_seq()) {
      return decode_from_sequence(node);
    }
    throw YAMLException::create_invalid_format("Rect2");
  } catch (const YAMLException&) {
    throw;
  } catch (const std::exception& e) {
    throw YAMLException::create_decode_error("Rect2", e.what());
  }
}

Variant Rect2VariantConverter::decode_from_map(const ryml::ConstNodeRef& node) const
{
  if (!node.has_child("position") || !node.has_child("size")) {
    throw YAMLException::create_missing_field("Rect2", "position, size");
  }

  const auto* vec2_converter = VariantConverterRegistry::get_converter(Variant::VECTOR2);
  Vector2 position = vec2_converter->decode(node["position"]).operator Vector2();
  Vector2 size = vec2_converter->decode(node["size"]).operator Vector2();

  return Rect2(position, size);
}

Variant Rect2VariantConverter::decode_from_sequence(const ryml::ConstNodeRef& node) const
{
  if (node.num_children() != 2) {
    throw YAMLException::create_invalid_sequence_length("Rect2", 2);
  }

  const auto* vec2_converter = VariantConverterRegistry::get_converter(Variant::VECTOR2);
  Vector2 position = vec2_converter->decode(node[0]).operator Vector2();
  Vector2 size = vec2_converter->decode(node[1]).operator Vector2();

  return Rect2(position, size);
}

Variant Rect2VariantConverter::decode_from_expanded(const ryml::ConstNodeRef& node) const
{
  real_t x = string_to_float<real_t>(node["x"].val());
  real_t y = string_to_float<real_t>(node["y"].val());
  real_t w = string_to_float<real_t>(node["w"].val());
  real_t h = string_to_float<real_t>(node["h"].val());

  return Rect2(x, y, w, h);
}

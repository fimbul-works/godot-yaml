#include "rect2i_yaml.h"
#include "../exception.h"
#include "../util_numeric.h"
#include "../variant_converter_registry.h"

using namespace godot;

void Rect2iVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const
{
  const Rect2i rect = v.operator Rect2i();
  YAMLFormat::Format fmt = format.get_format(Variant::RECT2I);

  if (fmt == YAMLFormat::SEQUENCE || fmt == YAMLFormat::FLOW_SEQUENCE) {
    emit_as_sequence(node, rect, format);
  } else {
    emit_as_map(node, rect, format);
  }
}

void Rect2iVariantConverter::emit_as_map(ryml::NodeRef& node, const Rect2i& rect, const YAMLFormat::View& format) const
{
  node |= ryml::MAP;

  if (format.get_format(Variant::RECT2I) == YAMLFormat::FLOW_MAP) {
    node |= ryml::FLOW_SL;
  }

  const auto* vec2i_converter = VariantConverterRegistry::get_converter(Variant::VECTOR2I);

  ryml::NodeRef pos_node = node["position"];
  vec2i_converter->encode(pos_node, rect.position, format);

  ryml::NodeRef size_node = node["size"];
  vec2i_converter->encode(size_node, rect.size, format);
}

void Rect2iVariantConverter::emit_as_sequence(ryml::NodeRef& node, const Rect2i& rect, const YAMLFormat::View& format) const
{
  node |= ryml::SEQ;

  if (format.get_format(Variant::RECT2I) == YAMLFormat::FLOW_SEQUENCE) {
    node |= ryml::FLOW_SL;
  }

  const auto* vec2i_converter = VariantConverterRegistry::get_converter(Variant::VECTOR2I);

  ryml::NodeRef pos_node = node.append_child();
  vec2i_converter->encode(pos_node, rect.position, format);

  ryml::NodeRef size_node = node.append_child();
  vec2i_converter->encode(size_node, rect.size, format);
}

Variant Rect2iVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  try {
    if (node.is_map()) {
      return decode_from_map(node);
    } else if (node.is_seq()) {
      return decode_from_sequence(node);
    }
    throw YAMLException::create_invalid_format("Rect2i");
  } catch (const YAMLException&) {
    throw;
  } catch (const std::exception& e) {
    throw YAMLException::create_decode_error("Rect2i", e.what());
  }
}

Variant Rect2iVariantConverter::decode_from_map(const ryml::ConstNodeRef& node) const
{
  if (!node.has_child("position") || !node.has_child("size")) {
    throw YAMLException::create_missing_field("Rect2i", "position, size");
  }

  const auto* vec2i_converter = VariantConverterRegistry::get_converter(Variant::VECTOR2I);
  Vector2i position = vec2i_converter->decode(node["position"]).operator Vector2i();
  Vector2i size = vec2i_converter->decode(node["size"]).operator Vector2i();

  return Rect2i(position, size);
}

Variant Rect2iVariantConverter::decode_from_sequence(const ryml::ConstNodeRef& node) const
{
  if (node.num_children() != 2) {
    throw YAMLException::create_invalid_sequence_length("Rect2i", 2);
  }

  const auto* vec2i_converter = VariantConverterRegistry::get_converter(Variant::VECTOR2I);
  Vector2i position = vec2i_converter->decode(node[0]).operator Vector2i();
  Vector2i size = vec2i_converter->decode(node[1]).operator Vector2i();

  return Rect2i(position, size);
}

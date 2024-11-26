#include "projection_yaml.h"
#include "../util_numeric.h"
#include "../variant_converter_registry.h"
#include "../yaml_exception.h"

using namespace godot;

void ProjectionVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const
{
  const Projection proj = v.operator Projection();

  // Check format and use appropriate encoding method
  switch (format.get_format(Variant::PROJECTION)) {
    case YAMLFormat::SEQUENCE:
      emit_as_sequence(node, proj, format);
      break;
    case YAMLFormat::BLOCK_MAP:
    case YAMLFormat::FLOW_MAP:
    default:
      emit_as_map(node, proj, format);
      break;
  }
}

void ProjectionVariantConverter::emit_as_map(ryml::NodeRef& node, const Projection& proj, const YAMLFormat::View& format) const
{
  node |= ryml::MAP;
  node |= ryml::FLOW_SL;

  // Encode each column as x, y, z, w
  emit_column(node["x"], proj.columns[0], format);
  emit_column(node["y"], proj.columns[1], format);
  emit_column(node["z"], proj.columns[2], format);
  emit_column(node["w"], proj.columns[3], format);
}

void ProjectionVariantConverter::emit_as_sequence(ryml::NodeRef& node, const Projection& proj, const YAMLFormat::View& format) const
{
  node |= ryml::SEQ;
  node |= ryml::FLOW_SL;

  // Emit columns in order
  for (int i = 0; i < 4; i++) {
    ryml::NodeRef col_node = node.append_child();
    emit_column(col_node, proj.columns[i], format);
  }
}

void ProjectionVariantConverter::emit_column(ryml::NodeRef& node, const Vector4& col, const YAMLFormat::View& format) const
{
  if (format.get_format(Variant::VECTOR4) == YAMLFormat::SEQUENCE) {
    // Emit as simple array format [x,y,z,w]
    node |= ryml::SEQ;
    node |= ryml::FLOW_SL;
    node.append_child() << float_to_string(col.x);
    node.append_child() << float_to_string(col.y);
    node.append_child() << float_to_string(col.z);
    node.append_child() << float_to_string(col.w);
  } else {
    // Use Vector4 converter for structured format
    const auto* vec4_converter = VariantConverterRegistry::get_converter(Variant::VECTOR4);
    vec4_converter->encode(node, col, format);
  }
}

Variant ProjectionVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  try {
    if (node.is_map()) {
      return decode_from_map(node);
    } else if (node.is_seq()) {
      return decode_from_sequence(node);
    }
    throw YAMLException::create_invalid_format("Projection");
  } catch (const YAMLException&) {
    throw;
  } catch (const std::exception& e) {
    throw YAMLException(String("Failed to decode Projection: ") + e.what());
  }
}

Variant ProjectionVariantConverter::decode_from_map(const ryml::ConstNodeRef& node) const
{
  if (!node.has_child("x") || !node.has_child("y") || !node.has_child("z") || !node.has_child("w")) {
    throw YAMLException::create_missing_field("Projection", "x, y, z, w");
  }

  // Create projection from columns
  Projection proj;
  proj.columns[0] = decode_column(node["x"]);
  proj.columns[1] = decode_column(node["y"]);
  proj.columns[2] = decode_column(node["z"]);
  proj.columns[3] = decode_column(node["w"]);

  return proj;
}

Variant ProjectionVariantConverter::decode_from_sequence(const ryml::ConstNodeRef& node) const
{
  if (node.num_children() != 4) {
    throw YAMLException::create_invalid_sequence_length("Projection", 4);
  }

  // Create projection from sequential columns
  Projection proj;
  for (int i = 0; i < 4; i++) {
    proj.columns[i] = decode_column(node[i]);
  }

  return proj;
}

Vector4 ProjectionVariantConverter::decode_column(const ryml::ConstNodeRef& node) const
{
  if (node.is_seq()) {
    return decode_array_column(node);
  } else {
    // Use Vector4 converter for structured format
    const auto* vec4_converter = VariantConverterRegistry::get_converter(Variant::VECTOR4);
    return vec4_converter->decode(node).operator Vector4();
  }
}

Vector4 ProjectionVariantConverter::decode_array_column(const ryml::ConstNodeRef& node) const
{
  if (node.num_children() != 4) {
    throw YAMLException("Projection column array must have exactly 4 elements");
  }

  return Vector4(
          string_to_float<real_t>(node[0].val()),
          string_to_float<real_t>(node[1].val()),
          string_to_float<real_t>(node[2].val()),
          string_to_float<real_t>(node[3].val()));
}

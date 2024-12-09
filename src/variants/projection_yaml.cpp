#include "projection_yaml.h"
#include "../converter_factory.h"
#include "../exception.h"
#include "../util_numeric.h"

using namespace godot;

ProjectionVariantConverter::ProjectionVariantConverter(ConverterFactory* factory) :
        vec4_converter(factory->create_converter_as<Vector4VariantConverter>(Variant::VECTOR4))
{
  ERR_FAIL_NULL(vec4_converter);
}

void ProjectionVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const YAMLStyle::View& style) const
{
  const Projection proj = v.operator Projection();

  if (!style.is_valid() || style.get_container_form() != YAMLStyle::FORM_SEQ) {
    emit_as_map(node, proj, style);
  } else {
    emit_as_sequence(node, proj, style);
  }
}

void ProjectionVariantConverter::emit_as_map(ryml::NodeRef& node, const Projection& proj, const YAMLStyle::View& style) const
{
  node |= ryml::MAP;

  // Flow style
  style.apply_flow_style(node);

  // Pass child styles for each column
  YAMLStyle::View x_style = style.is_valid() ? style.get_child("x") : YAMLStyle::View();
  YAMLStyle::View y_style = style.is_valid() ? style.get_child("y") : YAMLStyle::View();
  YAMLStyle::View z_style = style.is_valid() ? style.get_child("z") : YAMLStyle::View();
  YAMLStyle::View w_style = style.is_valid() ? style.get_child("w") : YAMLStyle::View();

  emit_column(node["x"], proj.columns[0], x_style);
  emit_column(node["y"], proj.columns[1], y_style);
  emit_column(node["z"], proj.columns[2], z_style);
  emit_column(node["w"], proj.columns[3], w_style);
}

void ProjectionVariantConverter::emit_as_sequence(ryml::NodeRef& node, const Projection& proj, const YAMLStyle::View& style) const
{
  node |= ryml::SEQ;

  // Flow style
  style.apply_flow_style(node);

  // Pass child styles using numeric indices
  for (int i = 0; i < 4; i++) {
    YAMLStyle::View col_style = style.is_valid() ? style.get_child(String::num_int64(i)) : YAMLStyle::View();
    ryml::NodeRef col_node = node.append_child();
    emit_column(col_node, proj.columns[i], col_style);
  }
}

void ProjectionVariantConverter::emit_column(ryml::NodeRef& node, const Vector4& col, const YAMLStyle::View& style) const
{
  vec4_converter->encode(node, col, style);
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
    throw YAMLException::create_decode_error("Projection", e.what());
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

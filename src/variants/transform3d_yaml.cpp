#include "transform3d_yaml.h"
#include "../yaml_exception.h"

using namespace godot;

Transform3DVariantConverter::Transform3DVariantConverter(YAML* yaml) :
        VariantConverter(yaml)
{
  vec_encoder = new Vector3VariantConverter(yaml);
  vec_encoder->set_format("flow"); // Use flow format for Vector3 components
}

Transform3DVariantConverter::~Transform3DVariantConverter()
{
  delete vec_encoder;
}

void Transform3DVariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  Transform3D transform = v.operator Transform3D();
  switch (format) {
    case Format::FLOW_MAP:
      emit_as_flow(node, transform);
      break;
    case Format::BLOCK_MAP:
      emit_as_block(node, transform);
      break;
  }
}

Variant Transform3DVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (!node.is_map()) {
    throw YAMLException::create_invalid_format("Transform3D");
  }

  // Check for required components
  const char* required_fields[] = { "x", "y", "z", "origin" };
  for (const char* field : required_fields) {
    if (!node.has_child(field)) {
      throw YAMLException::create_missing_field("Transform3D", field);
    }
  }

  try {
    // Decode each component
    Vector3 x = vec_encoder->decode(node["x"]);
    Vector3 y = vec_encoder->decode(node["y"]);
    Vector3 z = vec_encoder->decode(node["z"]);
    Vector3 origin = vec_encoder->decode(node["origin"]);

    // Construct the Transform3D
    Basis basis(x, y, z);
    return Transform3D(basis, origin);
  } catch (const std::exception& e) {
    throw YAMLException(String("Failed to decode Transform3D: ") + e.what());
  }
}

bool Transform3DVariantConverter::set_format(const String& format_str)
{
  if (format_str == "flow") {
    format = Format::FLOW_MAP;
    return true;
  } else if (format_str == "block") {
    format = Format::BLOCK_MAP;
    return true;
  }
  return false;
}

void Transform3DVariantConverter::emit_as_flow(ryml::NodeRef& node, const Transform3D& transform) const
{
  node |= ryml::MAP;
  node |= ryml::FLOW_SL;

  // Encode basis columns
  ryml::NodeRef x_node = node["x"];
  vec_encoder->encode(x_node, transform.basis.get_column(0));

  ryml::NodeRef y_node = node["y"];
  vec_encoder->encode(y_node, transform.basis.get_column(1));

  ryml::NodeRef z_node = node["z"];
  vec_encoder->encode(z_node, transform.basis.get_column(2));

  // Encode origin
  ryml::NodeRef origin_node = node["origin"];
  vec_encoder->encode(origin_node, transform.origin);
}

void Transform3DVariantConverter::emit_as_block(ryml::NodeRef& node, const Transform3D& transform) const
{
  node |= ryml::MAP;

  // Encode basis columns (without FLOW_SL for block format)
  ryml::NodeRef x_node = node["x"];
  vec_encoder->encode(x_node, transform.basis.get_column(0));

  ryml::NodeRef y_node = node["y"];
  vec_encoder->encode(y_node, transform.basis.get_column(1));

  ryml::NodeRef z_node = node["z"];
  vec_encoder->encode(z_node, transform.basis.get_column(2));

  // Encode origin
  ryml::NodeRef origin_node = node["origin"];
  vec_encoder->encode(origin_node, transform.origin);
}

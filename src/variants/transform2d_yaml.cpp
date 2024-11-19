#include "transform2d_yaml.h"
#include "../yaml_exception.h"

using namespace godot;

Transform2DVariantConverter::Transform2DVariantConverter(YAML* yaml) :
        VariantConverter(yaml)
{
  vec_encoder = new Vector2VariantConverter(yaml);
  vec_encoder->set_format("flow"); // Use flow format for Vector2 components
}

Transform2DVariantConverter::~Transform2DVariantConverter()
{
  delete vec_encoder;
}

void Transform2DVariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  Transform2D transform = v.operator Transform2D();
  emit_as_map(node, transform);
}

void Transform2DVariantConverter::emit_as_map(ryml::NodeRef& node, const Transform2D& transform) const
{
  node |= ryml::MAP;
  node |= ryml::FLOW_SL;

  // Encode the matrix columns (x, y components)
  ryml::NodeRef x_node = node["x"];
  vec_encoder->encode(x_node, transform.columns[0]);

  ryml::NodeRef y_node = node["y"];
  vec_encoder->encode(y_node, transform.columns[1]);

  // Encode the origin/translation component
  ryml::NodeRef origin_node = node["origin"];
  vec_encoder->encode(origin_node, transform.columns[2]);
}

Variant Transform2DVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (!node.is_map()) {
    throw YAMLException::create_invalid_format("Transform2D");
  }

  // Check for required components
  const char* required_fields[] = { "x", "y", "origin" };
  for (const char* field : required_fields) {
    if (!node.has_child(field)) {
      throw YAMLException::create_missing_field("Transform2D", field);
    }
  }

  try {
    // Decode each component
    Vector2 x = vec_encoder->decode(node["x"]);
    Vector2 y = vec_encoder->decode(node["y"]);
    Vector2 origin = vec_encoder->decode(node["origin"]);

    // Construct the Transform2D
    return Transform2D(x, y, origin);
  } catch (const std::exception& e) {
    throw YAMLException(String("Failed to decode Transform2D: ") + e.what());
  }
}

bool Transform2DVariantConverter::set_format(const String& format_str)
{
  // Format setting is delegated to the Vector2 encoder
  return vec_encoder->set_format(format_str);
}

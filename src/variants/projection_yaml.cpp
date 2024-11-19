#include "projection_yaml.h"
#include "../yaml_exception.h"

using namespace godot;

ProjectionVariantConverter::ProjectionVariantConverter(YAML* yaml) :
        VariantConverter(yaml)
{
  vec_encoder = new Vector4VariantConverter(yaml);
  vec_encoder->set_format("flow");
}

ProjectionVariantConverter::~ProjectionVariantConverter()
{
  delete vec_encoder;
}

void ProjectionVariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  Projection projection = v.operator Projection();
  emit_as_map(node, projection);
}

void ProjectionVariantConverter::emit_as_map(ryml::NodeRef& node, const Projection& projection) const
{
  node |= ryml::MAP;
  node |= ryml::FLOW_SL;

  // Encode each column of the projection matrix
  ryml::NodeRef x_node = node["x"];
  vec_encoder->encode(x_node, projection.columns[0]);

  ryml::NodeRef y_node = node["y"];
  vec_encoder->encode(y_node, projection.columns[1]);

  ryml::NodeRef z_node = node["z"];
  vec_encoder->encode(z_node, projection.columns[2]);

  ryml::NodeRef w_node = node["w"];
  vec_encoder->encode(w_node, projection.columns[3]);
}

Variant ProjectionVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (!node.is_map()) {
    throw YAMLException::create_invalid_format("Projection");
  }

  // Check for all required components
  const char* components[] = { "x", "y", "z", "w" };
  for (const char* comp : components) {
    if (!node.has_child(comp)) {
      throw YAMLException::create_missing_field("Projection", comp);
    }
  }

  try {
    Vector4 x = vec_encoder->decode(node["x"]);
    Vector4 y = vec_encoder->decode(node["y"]);
    Vector4 z = vec_encoder->decode(node["z"]);
    Vector4 w = vec_encoder->decode(node["w"]);
    return Projection(x, y, z, w);
  } catch (const std::exception& e) {
    throw YAMLException(String("Failed to decode Projection: ") + e.what());
  }
}

bool ProjectionVariantConverter::set_format(const String& format_str)
{
  return vec_encoder->set_format(format_str);
}

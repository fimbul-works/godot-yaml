#include "plane_yaml.h"
#include "../util_numeric.h"
#include "../yaml_exception.h"

using namespace godot;

PlaneVariantConverter::PlaneVariantConverter(YAML* yaml) :
        VariantConverter(yaml)
{
  vec_encoder = new Vector3VariantConverter(yaml);
  vec_encoder->set_format("flow");
}

PlaneVariantConverter::~PlaneVariantConverter()
{
  delete vec_encoder;
}

void PlaneVariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  Plane plane = v.operator Plane();
  emit_as_map(node, plane);
}

void PlaneVariantConverter::emit_as_map(ryml::NodeRef& node, const Plane& plane) const
{
  node |= ryml::MAP;
  node |= ryml::FLOW_SL;

  ryml::NodeRef normal_node = node["normal"];
  vec_encoder->encode(normal_node, plane.normal);

  node["d"] << float_to_string(plane.d);
}

Variant PlaneVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (!node.is_map()) {
    throw YAMLException::create_invalid_format("Plane");
  }

  if (!node.has_child("normal")) {
    throw YAMLException::create_missing_field("Plane", "normal");
  }
  if (!node.has_child("d")) {
    throw YAMLException::create_missing_field("Plane", "d");
  }

  try {
    Vector3 normal = vec_encoder->decode(node["normal"]);
    real_t d = string_to_float<real_t>(node["d"].val());
    return Plane(normal, d);
  } catch (const std::exception& e) {
    throw YAMLException(String("Failed to decode Plane: ") + e.what());
  }
}

bool PlaneVariantConverter::set_format(const String& format_str)
{
  return vec_encoder->set_format(format_str);
}

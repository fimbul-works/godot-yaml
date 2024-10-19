#include "yaml.h"

#include "variants/aabb_yaml.h"
#include "variants/basis_yaml.h"
#include "variants/color_yaml.h"
#include "variants/rect2_yaml.h"
#include "variants/rect2i_yaml.h"
#include "variants/vector2_yaml.h"
#include "variants/vector2i_yaml.h"
#include "variants/vector3_yaml.h"
#include "variants/vector3i_yaml.h"
#include "variants/vector4_yaml.h"
#include "variants/vector4i_yaml.h"

using namespace godot;

void YAML::register_type_encoders()
{
  type_encoders[Variant::AABB] = std::make_unique<AABBYAMLEncoder>();
  type_encoders[Variant::BASIS] = std::make_unique<BasisYAMLEncoder>();
  type_encoders[Variant::COLOR] = std::make_unique<ColorYAMLEncoder>();
  type_encoders[Variant::RECT2] = std::make_unique<Rect2YAMLEncoder>(this);
  type_encoders[Variant::RECT2I] = std::make_unique<Rect2iYAMLEncoder>(this);
  type_encoders[Variant::VECTOR2] = std::make_unique<Vector2YAMLEncoder>();
  type_encoders[Variant::VECTOR2I] = std::make_unique<Vector2iYAMLEncoder>();
  type_encoders[Variant::VECTOR3] = std::make_unique<Vector3YAMLEncoder>();
  type_encoders[Variant::VECTOR3I] = std::make_unique<Vector3iYAMLEncoder>();
  type_encoders[Variant::VECTOR4] = std::make_unique<Vector4YAMLEncoder>();
  type_encoders[Variant::VECTOR4I] = std::make_unique<Vector4iYAMLEncoder>();
}

bool YAML::set_format(Variant::Type type, const String& format)
{
  auto it = type_encoders.find(type);
  if (it != type_encoders.end()) {
    return it->second->set_format(format);
  }
  UtilityFunctions::printerr("YAML error: unsupported type for format setting - ", Variant::get_type_name(type));
  return false;
}

const IYAMLEncoder* YAML::get_encoder(Variant::Type type) const
{
  auto it = type_encoders.find(type);
  if (it != type_encoders.end()) {
    return it->second.get();
  }
  return nullptr;
}

bool YAML::has_matching_tag(const ryml::ConstNodeRef& node, const IYAMLEncoder* encoder) const
{
  return node.has_val_tag() && node.val_tag() == ryml::to_csubstr(std::string("!") + encoder->get_tag());
}

void YAML::set_node_tag(ryml::NodeRef& node, const IYAMLEncoder* encoder) const
{
  node.set_val_tag(ryml::to_csubstr(encoder->get_tag()));
}

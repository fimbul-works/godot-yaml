#include "yaml.h"

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
  type_encoders[Variant::COLOR] = std::make_unique<ColorYAMLEncoder>();
  type_encoders[Variant::RECT2] = std::make_unique<Rect2YAMLEncoder>();
  type_encoders[Variant::RECT2I] = std::make_unique<Rect2iYAMLEncoder>();
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
  UtilityFunctions::printerr("Unsupported type for format setting: ", Variant::get_type_name(type));
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

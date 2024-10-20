#include "yaml.h"

#include "variants/aabb_yaml.h"
#include "variants/basis_yaml.h"
#include "variants/color_yaml.h"
#include "variants/node_path_yaml.h"
#include "variants/packed_byte_array_yaml.h"
#include "variants/packed_color_array_yaml.h"
#include "variants/packed_float32_array_yaml.h"
#include "variants/packed_float64_array_yaml.h"
#include "variants/packed_int32_array_yaml.h"
#include "variants/packed_int64_array_yaml.h"
#include "variants/packed_string_array_yaml.h"
#include "variants/packed_vector2_array_yaml.h"
#include "variants/packed_vector3_array_yaml.h"
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
  type_encoders[Variant::NODE_PATH] = std::make_unique<NodePathYAMLEncoder>();
  type_encoders[Variant::PACKED_BYTE_ARRAY] = std::make_unique<PackedByteArrayYAMLEncoder>();
  type_encoders[Variant::PACKED_COLOR_ARRAY] = std::make_unique<PackedColorArrayYAMLEncoder>();
  type_encoders[Variant::PACKED_FLOAT32_ARRAY] = std::make_unique<PackedFloat32ArrayYAMLEncoder>();
  type_encoders[Variant::PACKED_FLOAT64_ARRAY] = std::make_unique<PackedFloat64ArrayYAMLEncoder>();
  type_encoders[Variant::PACKED_INT32_ARRAY] = std::make_unique<PackedInt32ArrayYAMLEncoder>();
  type_encoders[Variant::PACKED_INT64_ARRAY] = std::make_unique<PackedInt64ArrayYAMLEncoder>();
  type_encoders[Variant::PACKED_STRING_ARRAY] = std::make_unique<PackedStringArrayYAMLEncoder>();
  type_encoders[Variant::PACKED_VECTOR2_ARRAY] = std::make_unique<PackedVector2ArrayYAMLEncoder>();
  type_encoders[Variant::PACKED_VECTOR3_ARRAY] = std::make_unique<PackedVector3ArrayYAMLEncoder>();
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

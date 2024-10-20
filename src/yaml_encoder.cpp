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
#include "variants/plane_yaml.h"
#include "variants/projection_yaml.h"
#include "variants/quaternion_yaml.h"
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
  type_encoders[Variant::AABB] = std::make_unique<AABBYAMLEncoder>(this);
  type_encoders[Variant::BASIS] = std::make_unique<BasisYAMLEncoder>(this);
  type_encoders[Variant::COLOR] = std::make_unique<ColorYAMLEncoder>(this);
  type_encoders[Variant::NODE_PATH] = std::make_unique<NodePathYAMLEncoder>(this);
  type_encoders[Variant::PACKED_BYTE_ARRAY] = std::make_unique<PackedByteArrayYAMLEncoder>(this);
  type_encoders[Variant::PACKED_COLOR_ARRAY] = std::make_unique<PackedColorArrayYAMLEncoder>(this);
  type_encoders[Variant::PACKED_FLOAT32_ARRAY] = std::make_unique<PackedFloat32ArrayYAMLEncoder>(this);
  type_encoders[Variant::PACKED_FLOAT64_ARRAY] = std::make_unique<PackedFloat64ArrayYAMLEncoder>(this);
  type_encoders[Variant::PACKED_INT32_ARRAY] = std::make_unique<PackedInt32ArrayYAMLEncoder>(this);
  type_encoders[Variant::PACKED_INT64_ARRAY] = std::make_unique<PackedInt64ArrayYAMLEncoder>(this);
  type_encoders[Variant::PACKED_STRING_ARRAY] = std::make_unique<PackedStringArrayYAMLEncoder>(this);
  type_encoders[Variant::PACKED_VECTOR2_ARRAY] = std::make_unique<PackedVector2ArrayYAMLEncoder>(this);
  type_encoders[Variant::PACKED_VECTOR3_ARRAY] = std::make_unique<PackedVector3ArrayYAMLEncoder>(this);
  type_encoders[Variant::PLANE] = std::make_unique<PlaneYAMLEncoder>(this);
  type_encoders[Variant::PROJECTION] = std::make_unique<ProjectionYAMLEncoder>(this);
  type_encoders[Variant::QUATERNION] = std::make_unique<QuaternionYAMLEncoder>(this);
  type_encoders[Variant::RECT2] = std::make_unique<Rect2YAMLEncoder>(this);
  type_encoders[Variant::RECT2I] = std::make_unique<Rect2YAMLEncoder>(this);
  type_encoders[Variant::VECTOR2] = std::make_unique<Vector2YAMLEncoder>(this);
  type_encoders[Variant::VECTOR2I] = std::make_unique<Vector2iYAMLEncoder>(this);
  type_encoders[Variant::VECTOR3] = std::make_unique<Vector3YAMLEncoder>(this);
  type_encoders[Variant::VECTOR3I] = std::make_unique<Vector3iYAMLEncoder>(this);
  type_encoders[Variant::VECTOR4] = std::make_unique<Vector4YAMLEncoder>(this);
  type_encoders[Variant::VECTOR4I] = std::make_unique<Vector4iYAMLEncoder>(this);
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

const YAMLEncoder* YAML::get_encoder(Variant::Type type) const
{
  auto it = type_encoders.find(type);
  if (it != type_encoders.end()) {
    return it->second.get();
  }
  return nullptr;
}

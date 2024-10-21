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

void YAML::register_encoder(std::unique_ptr<YAMLEncoder> encoder)
{
  tag_to_encoder[encoder->get_full_tag()] = encoder.get();
  type_to_encoder[encoder->get_type()] = std::move(encoder);
}

void YAML::register_type_encoders()
{
  register_encoder(std::make_unique<AABBYAMLEncoder>(this));
  register_encoder(std::make_unique<AABBYAMLEncoder>(this));
  register_encoder(std::make_unique<BasisYAMLEncoder>(this));
  register_encoder(std::make_unique<ColorYAMLEncoder>(this));
  register_encoder(std::make_unique<NodePathYAMLEncoder>(this));
  register_encoder(std::make_unique<PackedByteArrayYAMLEncoder>(this));
  register_encoder(std::make_unique<PackedColorArrayYAMLEncoder>(this));
  register_encoder(std::make_unique<PackedFloat32ArrayYAMLEncoder>(this));
  register_encoder(std::make_unique<PackedFloat64ArrayYAMLEncoder>(this));
  register_encoder(std::make_unique<PackedInt32ArrayYAMLEncoder>(this));
  register_encoder(std::make_unique<PackedInt64ArrayYAMLEncoder>(this));
  register_encoder(std::make_unique<PackedStringArrayYAMLEncoder>(this));
  register_encoder(std::make_unique<PackedVector2ArrayYAMLEncoder>(this));
  register_encoder(std::make_unique<PackedVector3ArrayYAMLEncoder>(this));
  register_encoder(std::make_unique<PlaneYAMLEncoder>(this));
  register_encoder(std::make_unique<ProjectionYAMLEncoder>(this));
  register_encoder(std::make_unique<QuaternionYAMLEncoder>(this));
  register_encoder(std::make_unique<Rect2YAMLEncoder>(this));
  register_encoder(std::make_unique<Rect2iYAMLEncoder>(this));
  register_encoder(std::make_unique<Vector2YAMLEncoder>(this));
  register_encoder(std::make_unique<Vector2iYAMLEncoder>(this));
  register_encoder(std::make_unique<Vector3YAMLEncoder>(this));
  register_encoder(std::make_unique<Vector3iYAMLEncoder>(this));
  register_encoder(std::make_unique<Vector4YAMLEncoder>(this));
  register_encoder(std::make_unique<Vector4iYAMLEncoder>(this));

  tag_to_encoder[resource_encoder->get_full_tag()] = resource_encoder.get();
}

bool YAML::set_format(Variant::Type type, const String& format)
{
  auto it = type_to_encoder.find(type);
  if (it != type_to_encoder.end()) {
    return it->second->set_format(format);
  }
  UtilityFunctions::printerr("YAML error: unsupported type for format setting ", Variant::get_type_name(type));
  return false;
}

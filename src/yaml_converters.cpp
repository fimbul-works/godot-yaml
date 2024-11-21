#include "yaml.h"

// Include all variant converters
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
#include "variants/resource_yaml.h"
#include "variants/string_name_yaml.h"
#include "variants/transform2d_yaml.h"
#include "variants/transform3d_yaml.h"
#include "variants/vector2_yaml.h"
#include "variants/vector2i_yaml.h"
#include "variants/vector3_yaml.h"
#include "variants/vector3i_yaml.h"
#include "variants/vector4_yaml.h"
#include "variants/vector4i_yaml.h"

namespace godot {

void YAML::register_type_converters()
{
  // Initialize resource converter first
  resource_converter = std::make_unique<ResourceVariantConverter>(this);
  tag_to_converter[resource_converter->get_full_tag()] = resource_converter.get();

  // Register all variant converters
  register_converter(std::make_unique<AABBVariantConverter>(this));
  register_converter(std::make_unique<BasisVariantConverter>(this));
  register_converter(std::make_unique<ColorVariantConverter>(this));
  register_converter(std::make_unique<NodePathVariantConverter>(this));
  register_converter(std::make_unique<PackedByteArrayVariantConverter>(this));
  register_converter(std::make_unique<PackedColorArrayVariantConverter>(this));
  register_converter(std::make_unique<PackedFloat32ArrayVariantConverter>(this));
  register_converter(std::make_unique<PackedFloat64ArrayVariantConverter>(this));
  register_converter(std::make_unique<PackedInt32ArrayVariantConverter>(this));
  register_converter(std::make_unique<PackedInt64ArrayVariantConverter>(this));
  register_converter(std::make_unique<PackedStringArrayVariantConverter>(this));
  register_converter(std::make_unique<PackedVector2ArrayVariantConverter>(this));
  register_converter(std::make_unique<PackedVector3ArrayVariantConverter>(this));
  register_converter(std::make_unique<PlaneVariantConverter>(this));
  register_converter(std::make_unique<ProjectionVariantConverter>(this));
  register_converter(std::make_unique<QuaternionVariantConverter>(this));
  register_converter(std::make_unique<Rect2VariantConverter>(this));
  register_converter(std::make_unique<Rect2iVariantConverter>(this));
  register_converter(std::make_unique<StringNameVariantConverter>(this));
  register_converter(std::make_unique<Transform2DVariantConverter>(this));
  register_converter(std::make_unique<Transform3DVariantConverter>(this));
  register_converter(std::make_unique<Vector2VariantConverter>(this));
  register_converter(std::make_unique<Vector2iVariantConverter>(this));
  register_converter(std::make_unique<Vector3VariantConverter>(this));
  register_converter(std::make_unique<Vector3iVariantConverter>(this));
  register_converter(std::make_unique<Vector4VariantConverter>(this));
  register_converter(std::make_unique<Vector4iVariantConverter>(this));
}

} // namespace godot

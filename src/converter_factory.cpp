#include "converter_factory.h"

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
#include "variants/string_name_yaml.h"
#include "variants/transform2d_yaml.h"
#include "variants/transform3d_yaml.h"
#include "variants/vector2_yaml.h"
#include "variants/vector2i_yaml.h"
#include "variants/vector3_yaml.h"
#include "variants/vector3i_yaml.h"
#include "variants/vector4_yaml.h"
#include "variants/vector4i_yaml.h"

using namespace godot;

std::unique_ptr<VariantConverter> ConverterFactory::create_converter(Variant::Type type)
{
  switch (type) {
    case Variant::AABB:
      return std::make_unique<AABBVariantConverter>(this);
    case Variant::BASIS:
      return std::make_unique<BasisVariantConverter>(this);
    case Variant::COLOR:
      return std::make_unique<ColorVariantConverter>();
    case Variant::NODE_PATH:
      return std::make_unique<NodePathVariantConverter>();
    case Variant::PACKED_BYTE_ARRAY:
      return std::make_unique<PackedByteArrayVariantConverter>();
    case Variant::PACKED_COLOR_ARRAY:
      return std::make_unique<PackedColorArrayVariantConverter>(this);
    case Variant::PACKED_FLOAT32_ARRAY:
      return std::make_unique<PackedFloat32ArrayVariantConverter>();
    case Variant::PACKED_FLOAT64_ARRAY:
      return std::make_unique<PackedFloat64ArrayVariantConverter>();
    case Variant::PACKED_INT32_ARRAY:
      return std::make_unique<PackedInt32ArrayVariantConverter>();
    case Variant::PACKED_INT64_ARRAY:
      return std::make_unique<PackedInt64ArrayVariantConverter>();
    case Variant::PACKED_STRING_ARRAY:
      return std::make_unique<PackedStringArrayVariantConverter>();
    case Variant::PACKED_VECTOR2_ARRAY:
      return std::make_unique<PackedVector2ArrayVariantConverter>(this);
    case Variant::PACKED_VECTOR3_ARRAY:
      return std::make_unique<PackedVector3ArrayVariantConverter>(this);
    case Variant::PLANE:
      return std::make_unique<PlaneVariantConverter>(this);
    case Variant::PROJECTION:
      return std::make_unique<ProjectionVariantConverter>(this);
    case Variant::QUATERNION:
      return std::make_unique<QuaternionVariantConverter>();
    case Variant::RECT2:
      return std::make_unique<Rect2VariantConverter>(this);
    case Variant::RECT2I:
      return std::make_unique<Rect2iVariantConverter>(this);
    case Variant::STRING_NAME:
      return std::make_unique<StringNameVariantConverter>();
    case Variant::TRANSFORM2D:
      return std::make_unique<Transform2DVariantConverter>(this);
    case Variant::TRANSFORM3D:
      return std::make_unique<Transform3DVariantConverter>(this);
    case Variant::VECTOR2:
      return std::make_unique<Vector2VariantConverter>();
    case Variant::VECTOR2I:
      return std::make_unique<Vector2iVariantConverter>();
    case Variant::VECTOR3:
      return std::make_unique<Vector3VariantConverter>();
    case Variant::VECTOR3I:
      return std::make_unique<Vector3iVariantConverter>();
    case Variant::VECTOR4:
      return std::make_unique<Vector4VariantConverter>();
    case Variant::VECTOR4I:
      return std::make_unique<Vector4iVariantConverter>();
    default:
      return nullptr;
  }
}

std::unique_ptr<VariantConverter> ConverterFactory::create_converter_for_tag(const String& tag)
{
  return create_converter(get_type_for_tag(tag));
}

std::unordered_map<Variant::Type, std::unique_ptr<VariantConverter>> ConverterFactory::create_converter_set()
{
  std::unordered_map<Variant::Type, std::unique_ptr<VariantConverter>> converters;

  converters[Variant::AABB] = create_converter(Variant::AABB);
  converters[Variant::BASIS] = create_converter(Variant::BASIS);
  converters[Variant::COLOR] = create_converter(Variant::COLOR);
  converters[Variant::NODE_PATH] = create_converter(Variant::NODE_PATH);
  converters[Variant::PACKED_BYTE_ARRAY] = create_converter(Variant::PACKED_BYTE_ARRAY);
  converters[Variant::PACKED_COLOR_ARRAY] = create_converter(Variant::PACKED_COLOR_ARRAY);
  converters[Variant::PACKED_FLOAT32_ARRAY] = create_converter(Variant::PACKED_FLOAT32_ARRAY);
  converters[Variant::PACKED_FLOAT64_ARRAY] = create_converter(Variant::PACKED_FLOAT64_ARRAY);
  converters[Variant::PACKED_INT32_ARRAY] = create_converter(Variant::PACKED_INT32_ARRAY);
  converters[Variant::PACKED_INT64_ARRAY] = create_converter(Variant::PACKED_INT64_ARRAY);
  converters[Variant::PACKED_STRING_ARRAY] = create_converter(Variant::PACKED_STRING_ARRAY);
  converters[Variant::PACKED_VECTOR2_ARRAY] = create_converter(Variant::PACKED_VECTOR2_ARRAY);
  converters[Variant::PACKED_VECTOR3_ARRAY] = create_converter(Variant::PACKED_VECTOR3_ARRAY);
  converters[Variant::PLANE] = create_converter(Variant::PLANE);
  converters[Variant::PROJECTION] = create_converter(Variant::PROJECTION);
  converters[Variant::QUATERNION] = create_converter(Variant::QUATERNION);
  converters[Variant::RECT2] = create_converter(Variant::RECT2);
  converters[Variant::RECT2I] = create_converter(Variant::RECT2I);
  converters[Variant::STRING_NAME] = create_converter(Variant::STRING_NAME);
  converters[Variant::TRANSFORM2D] = create_converter(Variant::TRANSFORM2D);
  converters[Variant::TRANSFORM3D] = create_converter(Variant::TRANSFORM3D);
  converters[Variant::VECTOR2] = create_converter(Variant::VECTOR2);
  converters[Variant::VECTOR2I] = create_converter(Variant::VECTOR2I);
  converters[Variant::VECTOR3] = create_converter(Variant::VECTOR3);
  converters[Variant::VECTOR3I] = create_converter(Variant::VECTOR3I);
  converters[Variant::VECTOR4] = create_converter(Variant::VECTOR4);
  converters[Variant::VECTOR4I] = create_converter(Variant::VECTOR4I);

  return converters;
}

Variant::Type ConverterFactory::get_type_for_tag(const String& tag)
{
  if (tag.match(Vector2VariantConverter::TAG))
    return Variant::VECTOR2;
  if (tag.match(Vector2iVariantConverter::TAG))
    return Variant::VECTOR2I;
  if (tag.match(Vector3VariantConverter::TAG))
    return Variant::VECTOR3;
  if (tag.match(Vector3iVariantConverter::TAG))
    return Variant::VECTOR3I;
  if (tag.match(Vector4VariantConverter::TAG))
    return Variant::VECTOR4;
  if (tag.match(Vector4iVariantConverter::TAG))
    return Variant::VECTOR4I;
  if (tag.match(AABBVariantConverter::TAG))
    return Variant::AABB;
  if (tag.match(BasisVariantConverter::TAG))
    return Variant::BASIS;
  if (tag.match(ColorVariantConverter::TAG))
    return Variant::COLOR;
  if (tag.match(NodePathVariantConverter::TAG))
    return Variant::NODE_PATH;
  if (tag.match(PackedByteArrayVariantConverter::TAG))
    return Variant::PACKED_BYTE_ARRAY;
  if (tag.match(PackedColorArrayVariantConverter::TAG))
    return Variant::PACKED_COLOR_ARRAY;
  if (tag.match(PackedFloat32ArrayVariantConverter::TAG))
    return Variant::PACKED_FLOAT32_ARRAY;
  if (tag.match(PackedFloat64ArrayVariantConverter::TAG))
    return Variant::PACKED_FLOAT64_ARRAY;
  if (tag.match(PackedInt32ArrayVariantConverter::TAG))
    return Variant::PACKED_INT32_ARRAY;
  if (tag.match(PackedInt64ArrayVariantConverter::TAG))
    return Variant::PACKED_INT64_ARRAY;
  if (tag.match(PackedStringArrayVariantConverter::TAG))
    return Variant::PACKED_STRING_ARRAY;
  if (tag.match(PackedVector2ArrayVariantConverter::TAG))
    return Variant::PACKED_VECTOR2_ARRAY;
  if (tag.match(PackedVector3ArrayVariantConverter::TAG))
    return Variant::PACKED_VECTOR3_ARRAY;
  if (tag.match(PlaneVariantConverter::TAG))
    return Variant::PLANE;
  if (tag.match(ProjectionVariantConverter::TAG))
    return Variant::PROJECTION;
  if (tag.match(QuaternionVariantConverter::TAG))
    return Variant::QUATERNION;
  if (tag.match(Rect2VariantConverter::TAG))
    return Variant::RECT2;
  if (tag.match(Rect2iVariantConverter::TAG))
    return Variant::RECT2I;
  if (tag.match(StringNameVariantConverter::TAG))
    return Variant::STRING_NAME;
  if (tag.match(Transform2DVariantConverter::TAG))
    return Variant::TRANSFORM2D;
  if (tag.match(Transform3DVariantConverter::TAG))
    return Variant::TRANSFORM3D;

  return Variant::NIL; // Not found
}

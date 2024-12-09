#include "variant_converter_registry.h"
#include "exception.h"

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

void VariantConverterRegistry::initialize()
{
  if (is_initialized)
    return;
  initialize_registry();
  is_initialized = true;
}

void VariantConverterRegistry::cleanup()
{
  type_to_converter.clear();
  tag_to_converter.clear();
  is_initialized = false;
}

const VariantConverter* VariantConverterRegistry::get_converter(Variant::Type type)
{
  auto it = type_to_converter.find(type);
  if (it == type_to_converter.end()) {
    throw YAMLException(String("No converter found for type: ") + Variant::get_type_name(type));
  }
  return it->second.get();
}

const VariantConverter* VariantConverterRegistry::get_converter_by_tag(const String& tag)
{
  auto it = tag_to_converter.find(tag);
  return it != tag_to_converter.end() ? it->second : nullptr;
}

template <Variant::Type T>
static const VariantConverter* get_converter_checked(const char* type_name)
{
  if (!has_converter(T)) {
    throw YAMLException(String(type_name) + " converter not found in registry");
  }
  return get_converter(T);
}

void VariantConverterRegistry::register_converter(std::unique_ptr<VariantConverter> converter)
{
  auto* converter_ptr = converter.get();
  Variant::Type type = converter_ptr->get_type();
  String tag = converter_ptr->get_tag();

  type_to_converter[type] = std::move(converter);
  tag_to_converter[tag] = converter_ptr;
}

void VariantConverterRegistry::initialize_registry()
{
  init_math_converters();
  init_vector_converters();
  init_transform_converters();
  init_color_converters();
  init_array_converters();
  init_string_converters();
}

bool VariantConverterRegistry::has_converter(Variant::Type type)
{
  return type_to_converter.find(type) != type_to_converter.end();
}

bool VariantConverterRegistry::has_converter_for_tag(const String& tag)
{
  return tag_to_converter.find(tag) != tag_to_converter.end();
}

// Converter initialization implementations
void VariantConverterRegistry::init_vector_converters()
{
  register_converter(std::make_unique<Vector2VariantConverter>());
  register_converter(std::make_unique<Vector2iVariantConverter>());
  register_converter(std::make_unique<Vector3VariantConverter>());
  register_converter(std::make_unique<Vector3iVariantConverter>());
  register_converter(std::make_unique<Vector4VariantConverter>());
  register_converter(std::make_unique<Vector4iVariantConverter>());
}

void VariantConverterRegistry::init_transform_converters()
{
  register_converter(std::make_unique<Transform2DVariantConverter>());
  register_converter(std::make_unique<Transform3DVariantConverter>());
  register_converter(std::make_unique<BasisVariantConverter>());
  register_converter(std::make_unique<ProjectionVariantConverter>());
}

void VariantConverterRegistry::init_math_converters()
{
  register_converter(std::make_unique<AABBVariantConverter>());
  register_converter(std::make_unique<Rect2VariantConverter>());
  register_converter(std::make_unique<Rect2iVariantConverter>());
  register_converter(std::make_unique<PlaneVariantConverter>());
  register_converter(std::make_unique<QuaternionVariantConverter>());
}

void VariantConverterRegistry::init_color_converters()
{
  register_converter(std::make_unique<ColorVariantConverter>());
}

void VariantConverterRegistry::init_array_converters()
{
  register_converter(std::make_unique<PackedByteArrayVariantConverter>());
  register_converter(std::make_unique<PackedColorArrayVariantConverter>());
  register_converter(std::make_unique<PackedFloat32ArrayVariantConverter>());
  register_converter(std::make_unique<PackedFloat64ArrayVariantConverter>());
  register_converter(std::make_unique<PackedInt32ArrayVariantConverter>());
  register_converter(std::make_unique<PackedInt64ArrayVariantConverter>());
  register_converter(std::make_unique<PackedStringArrayVariantConverter>());
  register_converter(std::make_unique<PackedVector2ArrayVariantConverter>());
  register_converter(std::make_unique<PackedVector3ArrayVariantConverter>());
}

void VariantConverterRegistry::init_string_converters()
{
  register_converter(std::make_unique<StringNameVariantConverter>());
  register_converter(std::make_unique<NodePathVariantConverter>());
}

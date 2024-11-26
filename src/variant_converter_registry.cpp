#include "variant_converter_registry.h"
#include "exception.h"

#include "variants/aabb_yaml.h"
#include "variants/basis_yaml.h"
#include "variants/color_yaml.h"
#include "variants/node_path_yaml.h"
#include "variants/object_yaml.h"
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

namespace godot {

// Static member initialization
std::shared_mutex VariantConverterRegistry::s_registry_mutex;
std::once_flag VariantConverterRegistry::s_init_flag;
bool VariantConverterRegistry::s_is_initialized = false;
std::unordered_map<Variant::Type, std::unique_ptr<VariantConverter>> VariantConverterRegistry::s_type_to_converter;
std::unordered_map<String, VariantConverter*> VariantConverterRegistry::s_tag_to_converter;

const VariantConverter* VariantConverterRegistry::get_converter(Variant::Type type)
{
  std::shared_lock lock(s_registry_mutex);

  if (!s_is_initialized) {
    ERR_PRINT("VariantConverterRegistry accessed before initialization");
    return nullptr;
  }

  auto it = s_type_to_converter.find(type);
  if (it == s_type_to_converter.end()) {
    throw YAMLException(String("No converter found for type: ") + Variant::get_type_name(type));
  }
  return it->second.get();
}

const VariantConverter* VariantConverterRegistry::get_converter_by_tag(const String& tag)
{
  std::shared_lock lock(s_registry_mutex);

  if (!s_is_initialized) {
    ERR_PRINT("VariantConverterRegistry accessed before initialization");
    return nullptr;
  }

  auto it = s_tag_to_converter.find(tag);
  return it != s_tag_to_converter.end() ? it->second : nullptr;
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

  // Store converter and set up tag mapping
  s_type_to_converter[type] = std::move(converter);
  s_tag_to_converter[tag] = converter_ptr;
}

void VariantConverterRegistry::initialize_registry()
{
  std::call_once(s_init_flag, []() {
    std::unique_lock lock(s_registry_mutex);

    // Initialize all converter groups
    init_primitive_converters();
    init_math_converters();
    init_vector_converters();
    init_transform_converters();
    init_color_converters();
    init_array_converters();
    init_string_converters();
    init_object_converters();

    s_is_initialized = true;
  });
}

void VariantConverterRegistry::cleanup_registry()
{
  std::unique_lock lock(s_registry_mutex);

  s_tag_to_converter.clear();
  s_type_to_converter.clear();
  s_is_initialized = false;
}

bool VariantConverterRegistry::has_converter(Variant::Type type)
{
  std::shared_lock lock(s_registry_mutex);
  return s_type_to_converter.find(type) != s_type_to_converter.end();
}

bool VariantConverterRegistry::has_converter_for_tag(const String& tag)
{
  std::shared_lock lock(s_registry_mutex);
  return s_tag_to_converter.find(tag) != s_tag_to_converter.end();
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

void VariantConverterRegistry::init_primitive_converters()
{
  // RID cannot be serialized!
  // register_converter(std::make_unique<RIDVariantConverter>());
}

void VariantConverterRegistry::init_object_converters()
{
  // Resource and custom object converters
  register_converter(std::make_unique<ObjectVariantConverter>());
}

} // namespace godot

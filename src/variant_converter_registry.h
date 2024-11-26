#ifndef VARIANT_CONVERTER_REGISTRY_H
#define VARIANT_CONVERTER_REGISTRY_H

#include "string_hash.h"
#include "variant_converter.h"

#include <memory>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

namespace godot {

class VariantConverterRegistry {
  public:
  // Thread-safe converter access
  static const VariantConverter* get_converter(Variant::Type type);
  static const VariantConverter* get_converter_by_tag(const String& tag);

  // Initialization and cleanup (called by GDExtension)
  static void initialize_registry();
  static void cleanup_registry();

  // Testing/Debug helpers
  static bool has_converter(Variant::Type type);
  static bool has_converter_for_tag(const String& tag);

  // Easily retrieve converter for type
  template <Variant::Type T>
  static const VariantConverter* get_converter_for(const char* type_name)
  {
    const auto* converter = get_converter(T);
    if (!converter) {
      throw YAMLException(String(type_name) + " converter not found in registry");
    }
    return converter;
  }

  private:
  // Prevent instantiation
  VariantConverterRegistry() = delete;
  ~VariantConverterRegistry() = delete;

  // Thread safety
  static std::shared_mutex s_registry_mutex;
  static std::once_flag s_init_flag;
  static bool s_is_initialized;

  // Storage
  static std::unordered_map<Variant::Type, std::unique_ptr<VariantConverter>> s_type_to_converter;
  static std::unordered_map<String, VariantConverter*> s_tag_to_converter;

  // Registration helpers
  static void register_converter(std::unique_ptr<VariantConverter> converter);

  // Initialization helpers
  static void init_primitive_converters();
  static void init_math_converters();
  static void init_vector_converters();
  static void init_transform_converters();
  static void init_color_converters();
  static void init_array_converters();
  static void init_string_converters();
  static void init_object_converters();

  // Friends for initialization
  friend void initialize_yaml_module(ModuleInitializationLevel p_level);
  friend void uninitialize_yaml_module(ModuleInitializationLevel p_level);
};

} // namespace godot

#endif // VARIANT_CONVERTER_REGISTRY_H

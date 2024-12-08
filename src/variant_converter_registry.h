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
  static const VariantConverter* get_converter(Variant::Type type);
  static const VariantConverter* get_converter_by_tag(const String& tag);
  static void initialize_registry();
  static void cleanup_registry();
  static bool has_converter(Variant::Type type);
  static bool has_converter_for_tag(const String& tag);

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
  VariantConverterRegistry() = delete;
  ~VariantConverterRegistry() = delete;

  static std::shared_mutex s_registry_mutex;
  static std::atomic<bool> s_is_initialized;
  static std::once_flag s_init_flag;

  struct RegistryData {
    std::unordered_map<Variant::Type, std::unique_ptr<VariantConverter>> type_to_converter;
    std::unordered_map<String, VariantConverter*, StringHasher> tag_to_converter;
  };

  static std::unique_ptr<RegistryData> s_registry_data;

  static void register_converter(std::unique_ptr<VariantConverter> converter);
  static void ensure_initialized();

  // Initialization helpers remain the same
  static void init_primitive_converters();
  static void init_math_converters();
  static void init_vector_converters();
  static void init_transform_converters();
  static void init_color_converters();
  static void init_array_converters();
  static void init_string_converters();
};

} // namespace godot

#endif // VARIANT_CONVERTER_REGISTRY_H

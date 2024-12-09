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
  // Thread-local instance access
  static VariantConverterRegistry& get_instance()
  {
    static thread_local VariantConverterRegistry instance;
    return instance;
  }

  void initialize();
  void cleanup();

  const VariantConverter* get_converter(Variant::Type type);
  const VariantConverter* get_converter_by_tag(const String& tag);
  void initialize_registry();
  void cleanup_registry();
  bool has_converter(Variant::Type type);
  bool has_converter_for_tag(const String& tag);

  template <Variant::Type T>
  const VariantConverter* get_converter_for(const char* type_name)
  {
    const auto* converter = get_converter(T);
    if (!converter) {
      throw YAMLException(String(type_name) + " converter not found in registry");
    }
    return converter;
  }

  private:
  VariantConverterRegistry() { initialize(); }
  ~VariantConverterRegistry() { cleanup(); }

  std::unordered_map<Variant::Type, std::unique_ptr<VariantConverter>> type_to_converter;
  std::unordered_map<String, VariantConverter*, StringHasher> tag_to_converter;
  bool is_initialized { false };

  void register_converter(std::unique_ptr<VariantConverter> converter);

  void init_primitive_converters();
  void init_math_converters();
  void init_vector_converters();
  void init_transform_converters();
  void init_color_converters();
  void init_array_converters();
  void init_string_converters();
};

} // namespace godot

#endif // VARIANT_CONVERTER_REGISTRY_H

#ifndef CONVERTER_FACTORY_H
#define CONVERTER_FACTORY_H

#include "variant_converter.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>

#include <memory>
#include <unordered_map>

namespace godot {

class ConverterFactory {
  public:
  ConverterFactory() = default;
  ~ConverterFactory() = default;

  // Non-copyable to prevent accidental sharing
  ConverterFactory(const ConverterFactory&) = delete;
  ConverterFactory& operator=(const ConverterFactory&) = delete;

  // Creates a single converter for a specific type
  std::unique_ptr<VariantConverter> create_converter(Variant::Type type);

  // Template function to create and cast a converter to a specific type
  template <typename T>
  std::unique_ptr<T> create_converter_as(Variant::Type type)
  {
    auto base = create_converter(type);
    if (!base)
      return nullptr;

    T* derived = dynamic_cast<T*>(base.release());
    if (!derived) {
      // Handle error case - wrong type
      return nullptr;
    }

    return std::unique_ptr<T>(derived);
  }

  // Creates a single converter for a specific type
  std::unique_ptr<VariantConverter> create_converter_for_tag(const String& tag);

  // Creates a complete set of converters mapped by Variant type
  std::unordered_map<Variant::Type, std::unique_ptr<VariantConverter>> create_converter_set();

  private:
  Variant::Type get_type_for_tag(const String& tag);
};

} // namespace godot

#endif // CONVERTER_FACTORY_H

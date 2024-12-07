#ifndef VARIANT_CONVERTER_H
#define VARIANT_CONVERTER_H

#include "style.h"

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <ryml.hpp>

#define DEFINE_YAML_TAG(TAG_VALUE, VARIANT_TYPE)                 \
  static constexpr const char* TAG = TAG_VALUE;                  \
  static constexpr const char* FULL_TAG = "!" TAG_VALUE;         \
  const char* get_tag() const override { return TAG; }           \
  const char* get_full_tag() const override { return FULL_TAG; } \
  const Variant::Type get_type() const override { return VARIANT_TYPE; }

namespace godot {

class VariantConverter {
  public:
  virtual ~VariantConverter() = default;

  // Tag identification
  virtual const char* get_tag() const = 0;
  virtual const char* get_full_tag() const = 0;
  virtual const Variant::Type get_type() const = 0;

  // Pure virtual encode method that derived classes must implement
  virtual void encode(ryml::NodeRef& node, const Variant& v, const Ref<YAMLStyle>& style) const = 0;

  // Pure virtual decode method
  virtual Variant decode(const ryml::ConstNodeRef& node) const = 0;
};

} // namespace godot

#endif // VARIANT_CONVERTER_H

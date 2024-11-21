#ifndef VARIANT_CONVERTER_H
#define VARIANT_CONVERTER_H

#include <godot_cpp/variant/variant.hpp>
#include <ryml.hpp>

namespace std {
template <>
struct hash<godot::String> {
  size_t operator()(const godot::String& str) const
  {
    // Use Godot's built-in hash function
    return str.hash();
  }
};

template <>
struct equal_to<godot::String> {
  bool operator()(const godot::String& lhs, const godot::String& rhs) const
  {
    return lhs == rhs;
  }
};
}

#define DEFINE_YAML_TAG(TAG_VALUE, VARIANT_TYPE)                 \
  static constexpr const char* TAG = TAG_VALUE;                  \
  static constexpr const char* FULL_TAG = "!" TAG_VALUE;         \
  const char* get_tag() const override { return TAG; }           \
  const char* get_full_tag() const override { return FULL_TAG; } \
  const Variant::Type get_type() const override { return VARIANT_TYPE; }

namespace godot {

class YAML;

class VariantConverter {
  public:
  VariantConverter(YAML* yaml) :
          m_yaml(yaml) { }
  virtual ~VariantConverter() = default;

  // Tag identification
  virtual const char* get_tag() const = 0;
  virtual const char* get_full_tag() const = 0;
  virtual const Variant::Type get_type() const = 0;

  // Conversion methods
  virtual void encode(ryml::NodeRef& node, const Variant& v) const = 0;
  virtual Variant decode(const ryml::ConstNodeRef& node) const = 0;

  // Format configuration
  virtual bool set_format(const String& format) = 0;

  protected:
  YAML* m_yaml;
};

} // namespace godot

#endif // VARIANT_CONVERTER_H

#ifndef YAML_ENCODER_H
#define YAML_ENCODER_H

#include <godot_cpp/variant/variant.hpp>
#include <ryml.hpp>

namespace godot {

struct IYAMLEncoder {
  public:
  virtual const char* get_tag() const = 0;

  virtual void encode(ryml::NodeRef& node, const Variant& v) const = 0;
  virtual Variant decode(const ryml::ConstNodeRef& node) const = 0;

  virtual bool set_format(const String& format) = 0;

  virtual ~IYAMLEncoder() = default;
};

} // namespace godot

#endif // YAML_ENCODER_H

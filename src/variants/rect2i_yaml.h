#ifndef RECT2I_YAML_H
#define RECT2I_YAML_H

#include "vector2i_yaml.h"
#include "yaml.h"

namespace godot {

class Rect2iYAMLEncoder : public IYAMLEncoder {
  public:
  Rect2iYAMLEncoder();

  const char* get_tag() const override;

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  bool set_format(const String& format_str) override;

  private:
  void emit_as_map(ryml::NodeRef& node, const Rect2i& rect) const;

  Vector2iYAMLEncoder vec2i_encoder;
};

} // namespace godot

#endif // RECT2I_YAML_H

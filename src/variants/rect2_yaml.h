#ifndef RECT2_YAML_H
#define RECT2_YAML_H

#include "vector2_yaml.h"
#include "yaml.h"

namespace godot {

class Rect2VariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("Rect2", Variant::RECT2)

  Rect2VariantConverter(YAML* yaml);
  ~Rect2VariantConverter();

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  bool set_format(const String& format_str) override;

  private:
  void emit_as_map(ryml::NodeRef& node, const Rect2& rect) const;

  Vector2VariantConverter* vec_encoder;
};

} // namespace godot

#endif // RECT2_YAML_H

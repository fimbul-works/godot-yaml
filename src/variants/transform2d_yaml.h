#ifndef TRANSFORM2D_YAML_H
#define TRANSFORM2D_YAML_H

#include "../variant_converter.h"
#include "vector2_yaml.h"

namespace godot {

class Transform2DVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("Transform2D", Variant::TRANSFORM2D)

  Transform2DVariantConverter(YAML* yaml);
  ~Transform2DVariantConverter();

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;
  bool set_format(const String& format) override;

  private:
  void emit_as_map(ryml::NodeRef& node, const Transform2D& transform) const;
  Vector2VariantConverter* vec_encoder;
};

} // namespace godot

#endif // TRANSFORM2D_YAML_H

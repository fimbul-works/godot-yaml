#ifndef TRANSFORM3D_YAML_H
#define TRANSFORM3D_YAML_H

#include "vector3_yaml.h"
#include "yaml.h"

namespace godot {

class Transform3DVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("Transform3D", Variant::TRANSFORM3D)

  Transform3DVariantConverter(YAML* yaml);
  ~Transform3DVariantConverter();

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  bool set_format(const String& format_str) override;

  private:
  void emit_as_map(ryml::NodeRef& node, const Transform3D& transform) const;

  Vector3VariantConverter* vec_encoder;
};

} // namespace godot

#endif // TRANSFORM3D_YAML_H

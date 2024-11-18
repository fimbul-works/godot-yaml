#ifndef PLANE_YAML_H
#define PLANE_YAML_H

#include "vector3_yaml.h"
#include "yaml.h"

namespace godot {

class PlaneVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("Plane", Variant::PLANE)

  PlaneVariantConverter(YAML* yaml);
  ~PlaneVariantConverter();

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  bool set_format(const String& format_str) override;

  private:
  void emit_as_map(ryml::NodeRef& node, const Plane& aabb) const;

  Vector3VariantConverter* vec_encoder;
};

} // namespace godot

#endif // PLANE_YAML_H

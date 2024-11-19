#ifndef PLANE_YAML_H
#define PLANE_YAML_H

#include "../variant_converter.h"
#include "vector3_yaml.h"

namespace godot {

class PlaneVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("Plane", Variant::PLANE)

  PlaneVariantConverter(YAML* yaml);
  ~PlaneVariantConverter();

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;
  bool set_format(const String& format) override;

  private:
  void emit_as_map(ryml::NodeRef& node, const Plane& plane) const;
  Vector3VariantConverter* vec_encoder;
};

} // namespace godot

#endif // PLANE_YAML_H

#ifndef PROJECTION_YAML_H
#define PROJECTION_YAML_H

#include "../variant_converter.h"
#include "vector4_yaml.h"

namespace godot {

class ProjectionVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("Projection", Variant::PROJECTION)

  ProjectionVariantConverter(YAML* yaml);
  ~ProjectionVariantConverter();

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;
  bool set_format(const String& format) override;

  private:
  void emit_as_map(ryml::NodeRef& node, const Projection& projection) const;
  Vector4VariantConverter* vec_encoder;
};

} // namespace godot

#endif // PROJECTION_YAML_H

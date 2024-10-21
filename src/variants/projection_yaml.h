#ifndef PROJECTION_YAML_H
#define PROJECTION_YAML_H

#include "vector4_yaml.h"
#include "yaml.h"

namespace godot {

class ProjectionYAMLEncoder : public YAMLEncoder {
  public:
  DEFINE_YAML_TAG("Projection", Variant::PROJECTION)

  ProjectionYAMLEncoder(YAML* yaml);
  ~ProjectionYAMLEncoder();

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  bool set_format(const String& format_str) override;

  private:
  void emit_as_map(ryml::NodeRef& node, const Projection& basis) const;

  Vector4YAMLEncoder* vec_encoder;
};

} // namespace godot

#endif // PROJECTION_YAML_H

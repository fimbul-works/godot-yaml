#ifndef BASIS_YAML_H
#define BASIS_YAML_H

#include "vector3_yaml.h"
#include "yaml.h"
#include "yaml_encoder.h"

namespace godot {

class BasisYAMLEncoder : public YAMLEncoder {
  public:
  DEFINE_YAML_TAG("Basis", Variant::BASIS)

  BasisYAMLEncoder(YAML* yaml);
  ~BasisYAMLEncoder();

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  bool set_format(const String& format_str) override;

  private:
  void emit_as_map(ryml::NodeRef& node, const Basis& basis) const;

  Vector3YAMLEncoder* vec_encoder;
};

} // namespace godot

#endif // BASIS_YAML_H

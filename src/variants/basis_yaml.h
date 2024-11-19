#ifndef BASIS_YAML_H
#define BASIS_YAML_H

#include "../variant_converter.h"
#include "vector3_yaml.h"

namespace godot {

class BasisVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("Basis", Variant::BASIS)

  BasisVariantConverter(YAML* yaml);
  ~BasisVariantConverter();

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;
  bool set_format(const String& format) override;

  private:
  void emit_as_map(ryml::NodeRef& node, const Basis& basis) const;
  Vector3VariantConverter* vec_encoder;
};

} // namespace godot

#endif // BASIS_YAML_H

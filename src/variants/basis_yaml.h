#ifndef BASIS_YAML_H
#define BASIS_YAML_H

#include "../variant_converter.h"

namespace godot {

class BasisVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("Basis", Variant::BASIS)

  void encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  private:
  void emit_as_map(ryml::NodeRef& node, const Basis& basis, const YAMLFormat::View& format) const;
  void emit_as_sequence(ryml::NodeRef& node, const Basis& basis, const YAMLFormat::View& format) const;
  Variant decode_from_map(const ryml::ConstNodeRef& node) const;
  Variant decode_from_sequence(const ryml::ConstNodeRef& node) const;
  const VariantConverter* get_vec3_converter() const;
};

} // namespace godot

#endif // BASIS_YAML_H

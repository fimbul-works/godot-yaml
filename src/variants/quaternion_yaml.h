#ifndef QUATERNION_YAML_H
#define QUATERNION_YAML_H

#include "../variant_converter.h"

namespace godot {

class QuaternionVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("Quaternion", Variant::QUATERNION)

  void encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  private:
  void emit_as_map(ryml::NodeRef& node, const Quaternion& quat, const YAMLFormat::View& format) const;
  void emit_as_sequence(ryml::NodeRef& node, const Quaternion& quat, const YAMLFormat::View& format) const;

  Variant decode_from_map(const ryml::ConstNodeRef& node) const;
  Variant decode_from_sequence(const ryml::ConstNodeRef& node) const;
};

} // namespace godot

#endif // QUATERNION_YAML_H

#ifndef QUATERNION_YAML_H
#define QUATERNION_YAML_H

#include "../variant_converter.h"

namespace godot {

class QuaternionVariantConverter : public VariantConverter {
  enum class Format {
    FLOW_MAP, // {x: 0, y: 0, z: 0, w: 1}
    SEQUENCE // [0, 0, 0, 1]
  };

  public:
  DEFINE_YAML_TAG("Quaternion", Variant::QUATERNION)

  QuaternionVariantConverter(YAML* yaml);

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;
  bool set_format(const String& format) override;

  private:
  Format format = Format::FLOW_MAP;

  void emit_as_flow(ryml::NodeRef& node, const Quaternion& quat) const;
  void emit_as_sequence(ryml::NodeRef& node, const Quaternion& quat) const;
};

} // namespace godot

#endif // QUATERNION_YAML_H

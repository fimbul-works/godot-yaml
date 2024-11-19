#ifndef TRANSFORM3D_YAML_H
#define TRANSFORM3D_YAML_H

#include "../variant_converter.h"
#include "vector3_yaml.h"

namespace godot {

class Transform3DVariantConverter : public VariantConverter {
  enum class Format {
    FLOW_MAP, // All components on single line
    BLOCK_MAP // Components on separate lines
  };

  public:
  DEFINE_YAML_TAG("Transform3D", Variant::TRANSFORM3D)

  Transform3DVariantConverter(YAML* yaml);
  ~Transform3DVariantConverter();

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;
  bool set_format(const String& format) override;

  private:
  Format format = Format::FLOW_MAP;
  Vector3VariantConverter* vec_encoder;

  void emit_as_flow(ryml::NodeRef& node, const Transform3D& transform) const;
  void emit_as_block(ryml::NodeRef& node, const Transform3D& transform) const;
};

} // namespace godot

#endif // TRANSFORM3D_YAML_H

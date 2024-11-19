#ifndef VECTOR3_YAML_H
#define VECTOR3_YAML_H

#include "../variant_converter.h"
#include "../yaml_exception.h"

namespace godot {

class Vector3VariantConverter : public VariantConverter {
  enum class Format {
    FLOW_MAP, // {x: 0, y: 0, z: 0}
    BLOCK_MAP, // x: 0\ny: 0\nz: 0
    SEQUENCE, // [0, 0, 0]
    INLINE // "(0, 0, 0)"
  };

  public:
  DEFINE_YAML_TAG("Vector3", Variant::VECTOR3)

  Vector3VariantConverter(YAML* yaml);

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;
  bool set_format(const String& format) override;

  private:
  Format format = Format::FLOW_MAP;

  void emit_as_flow(ryml::NodeRef& node, const Vector3& vec) const;
  void emit_as_block(ryml::NodeRef& node, const Vector3& vec) const;
  void emit_as_sequence(ryml::NodeRef& node, const Vector3& vec) const;
  void emit_as_inline(ryml::NodeRef& node, const Vector3& vec) const;

  Variant decode_map(const ryml::ConstNodeRef& node) const;
  Variant decode_sequence(const ryml::ConstNodeRef& node) const;
  Variant decode_inline(const ryml::csubstr& val) const;
};

} // namespace godot

#endif // VECTOR3_YAML_H

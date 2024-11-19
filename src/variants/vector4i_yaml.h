#ifndef VECTOR4I_YAML_H
#define VECTOR4I_YAML_H

#include "../variant_converter.h"
#include "../yaml_exception.h"

namespace godot {

class Vector4iVariantConverter : public VariantConverter {
  enum class Format {
    FLOW_MAP, // {x: 0, y: 0, z: 0, w: 0}
    BLOCK_MAP, // x: 0\ny: 0\nz: 0\nw: 0
    SEQUENCE, // [0, 0, 0, 0]
    INLINE // "(0, 0, 0, 0)"
  };

  public:
  DEFINE_YAML_TAG("Vector4i", Variant::VECTOR4I)

  Vector4iVariantConverter(YAML* yaml);

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;
  bool set_format(const String& format) override;

  private:
  Format format = Format::FLOW_MAP;

  void emit_as_flow(ryml::NodeRef& node, const Vector4i& vec) const;
  void emit_as_block(ryml::NodeRef& node, const Vector4i& vec) const;
  void emit_as_sequence(ryml::NodeRef& node, const Vector4i& vec) const;
  void emit_as_inline(ryml::NodeRef& node, const Vector4i& vec) const;

  Variant decode_map(const ryml::ConstNodeRef& node) const;
  Variant decode_sequence(const ryml::ConstNodeRef& node) const;
  Variant decode_inline(const ryml::csubstr& val) const;
};

} // namespace godot

#endif // VECTOR4I_YAML_H

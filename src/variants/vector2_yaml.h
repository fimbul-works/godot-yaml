#ifndef VECTOR2_YAML_H
#define VECTOR2_YAML_H

#include "../variant_converter.h"
#include "../yaml_exception.h"

namespace godot {

class Vector2VariantConverter : public VariantConverter {
  enum class Format {
    FLOW_MAP, // {x: 0, y: 0}
    BLOCK_MAP, // x: 0\ny: 0
    SEQUENCE, // [0, 0]
    INLINE // "(0, 0)"
  };

  public:
  DEFINE_YAML_TAG("Vector2", Variant::VECTOR2)

  Vector2VariantConverter(YAML* yaml);

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;
  bool set_format(const String& format) override;

  private:
  Format format = Format::FLOW_MAP;

  void emit_as_flow(ryml::NodeRef& node, const Vector2& vec) const;
  void emit_as_block(ryml::NodeRef& node, const Vector2& vec) const;
  void emit_as_sequence(ryml::NodeRef& node, const Vector2& vec) const;
  void emit_as_inline(ryml::NodeRef& node, const Vector2& vec) const;

  Variant decode_map(const ryml::ConstNodeRef& node) const;
  Variant decode_sequence(const ryml::ConstNodeRef& node) const;
  Variant decode_inline(const ryml::csubstr& val) const;
};

} // namespace godot

#endif // VECTOR2_YAML_H

#ifndef VECTOR2_YAML_H
#define VECTOR2_YAML_H

#include "yaml.h"
#include "yaml_encoder.h"

namespace godot {

class Vector2VariantConverter : public VariantConverter {
  enum class Format {
    FLOW_MAP,
    SEQUENCE
  };

  public:
  DEFINE_YAML_TAG("Vector2", Variant::VECTOR2)

  Vector2VariantConverter(YAML* yaml);

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  bool set_format(const String& format_str) override;

  private:
  Format format = Format::FLOW_MAP;

  void emit_as_flow(ryml::NodeRef& node, const Vector2& vec) const;
  void emit_as_sequence(ryml::NodeRef& node, const Vector2& vec) const;
};

} // namespace godot

#endif // VECTOR2_YAML_H

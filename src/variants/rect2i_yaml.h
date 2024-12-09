#ifndef RECT2I_YAML_H
#define RECT2I_YAML_H

#include "../converter_factory.h"
#include "../variant_converter.h"
#include "vector2i_yaml.h"

namespace godot {

class Rect2iVariantConverter : public VariantConverter {
  public:
  explicit Rect2iVariantConverter(ConverterFactory* factory);

  DEFINE_YAML_TAG("Rect2i", Variant::RECT2I)

  void encode(ryml::NodeRef& node, const Variant& v, const YAMLStyle::View& style) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  private:
  void emit_as_map(ryml::NodeRef& node, const Rect2i& rect, const YAMLStyle::View& style) const;
  void emit_as_sequence(ryml::NodeRef& node, const Rect2i& rect, const YAMLStyle::View& style) const;

  Variant decode_from_map(const ryml::ConstNodeRef& node) const;
  Variant decode_from_sequence(const ryml::ConstNodeRef& node) const;

  std::unique_ptr<Vector2iVariantConverter> vec2i_converter;
};

} // namespace godot

#endif // RECT2I_YAML_H

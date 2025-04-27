#ifndef COLOR_YAML_H
#define COLOR_YAML_H

#include "../variant_converter.h"

namespace godot {

class ColorVariantConverter : public VariantConverter {
public:
	DEFINE_YAML_TAG("Color", Variant::COLOR)

	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;
	Variant decode(const ryml::ConstNodeRef &node, ParserContext *context) const override;

private:
	void emit_as_map(ryml::NodeRef &node, const Color &color, const YAMLStyle::View &style) const;
	void emit_as_sequence(ryml::NodeRef &node, const Color &color, const YAMLStyle::View &style) const;

	Variant decode_map(const ryml::ConstNodeRef &node, ParserContext *context) const;
	Variant decode_sequence(const ryml::ConstNodeRef &node, ParserContext *context) const;
	Variant decode_string(const ryml::ConstNodeRef &node) const;

	void check_negative(real_t r, real_t g, real_t b, real_t a, const ryml::ConstNodeRef &node) const;
};

} // namespace godot

#endif // COLOR_YAML_H

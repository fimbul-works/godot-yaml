#ifndef COLOR_YAML_H
#define COLOR_YAML_H

#include "../variant_converter.h"

namespace godot {

class ColorVariantConverter : public VariantConverter {
public:
	DEFINE_YAML_TAG("Color", Variant::COLOR)

	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;
	Variant decode(const ryml::ConstNodeRef &node) const override;

private:
	void emit_as_hex(ryml::NodeRef &node, const Color &color, bool with_alpha, const char *prefix) const;
	void emit_as_map(ryml::NodeRef &node, const Color &color, const YAMLStyle::View &style) const;
	void emit_as_sequence(ryml::NodeRef &node, const Color &color, const YAMLStyle::View &style) const;

	Variant decode_hex(const ryml::ConstNodeRef &node) const;
	Variant decode_map(const ryml::ConstNodeRef &node) const;
	Variant decode_sequence(const ryml::ConstNodeRef &node) const;
	Color parse_hex_components(const String &hex_str, int offset, size_t expected_length, const ryml::ConstNodeRef &node) const;

	ryml::csubstr color_to_hex(const Color &color, bool with_alpha, const char *prefix) const;

	static constexpr int HEX_STRING_LENGTH = 7; // #RRGGBB
	static constexpr int HEX_NUMBER_LENGTH = 8; // 0xRRGGBB
	static constexpr int HEX_ALPHA_EXTRA = 2; // Additional AA
	static constexpr float COLOR_COMPONENT_MAX = 255.0f;
};

} // namespace godot

#endif // COLOR_YAML_H

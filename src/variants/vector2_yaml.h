#ifndef VECTOR2_YAML_H
#define VECTOR2_YAML_H

#include "../variant_converter.h"

namespace godot {

class Vector2VariantConverter : public VariantConverter {
public:
	DEFINE_YAML_TAG("Vector2", Variant::VECTOR2)

	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;
	Variant decode(const ryml::ConstNodeRef &node, ParserContext *context) const override;

private:
	Vector2 decode_from_map(const ryml::ConstNodeRef &node, ParserContext *context) const;
	Vector2 decode_from_sequence(const ryml::ConstNodeRef &node, ParserContext *context) const;
};

} // namespace godot

#endif // VECTOR2_YAML_H

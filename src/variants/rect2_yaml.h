#ifndef RECT2_YAML_H
#define RECT2_YAML_H

#include "../converter_factory.h"
#include "../variant_converter.h"
#include "vector2_yaml.h"

namespace godot {

class Rect2VariantConverter : public VariantConverter {
public:
	explicit Rect2VariantConverter(ConverterFactory *factory);

	DEFINE_YAML_TAG("Rect2", Variant::RECT2)

	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;
	Variant decode(const ryml::ConstNodeRef &node, ParserContext *context) const override;

private:
	Rect2 decode_from_map(const ryml::ConstNodeRef &node, ParserContext *context) const;
	Rect2 decode_from_sequence(const ryml::ConstNodeRef &node, ParserContext *context) const;

	std::unique_ptr<Vector2VariantConverter> vec2_converter;
};

} // namespace godot

#endif // RECT2_YAML_H

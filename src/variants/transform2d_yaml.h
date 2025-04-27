#ifndef TRANSFORM2D_YAML_H
#define TRANSFORM2D_YAML_H

#include "../converter_factory.h"
#include "../variant_converter.h"
#include "vector2_yaml.h"

namespace godot {

class Transform2DVariantConverter : public VariantConverter {
public:
	explicit Transform2DVariantConverter(ConverterFactory *factory);

	DEFINE_YAML_TAG("Transform2D", Variant::TRANSFORM2D)

	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;
	Variant decode(const ryml::ConstNodeRef &node, ParserContext *context) const override;

private:
	Transform2D decode_from_map(const ryml::ConstNodeRef &node, ParserContext *context) const;
	Transform2D decode_from_sequence(const ryml::ConstNodeRef &node, ParserContext *context) const;

	std::unique_ptr<Vector2VariantConverter> vec2_converter;
};

} // namespace godot

#endif // TRANSFORM2D_YAML_H

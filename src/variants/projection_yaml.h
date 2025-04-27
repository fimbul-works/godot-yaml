#ifndef PROJECTION_YAML_H
#define PROJECTION_YAML_H

#include "../converter_factory.h"
#include "../variant_converter.h"
#include "vector4_yaml.h"

namespace godot {

class ProjectionVariantConverter : public VariantConverter {
public:
	explicit ProjectionVariantConverter(ConverterFactory *factory);

	DEFINE_YAML_TAG("Projection", Variant::PROJECTION)

	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;
	Variant decode(const ryml::ConstNodeRef &node, ParserContext *context) const override;

private:
	Variant decode_from_map(const ryml::ConstNodeRef &node, ParserContext *context) const;
	Variant decode_from_sequence(const ryml::ConstNodeRef &node, ParserContext *context) const;

	std::unique_ptr<Vector4VariantConverter> vec4_converter;
};

} // namespace godot

#endif // PROJECTION_YAML_H

#ifndef AABB_YAML_H
#define AABB_YAML_H

#include "../converter_factory.h"
#include "../variant_converter.h"
#include "vector3_yaml.h"

namespace godot {

class AABBVariantConverter : public VariantConverter {
public:
	explicit AABBVariantConverter(ConverterFactory *factory);

	DEFINE_YAML_TAG("AABB", Variant::AABB)

	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;
	Variant decode(const ryml::ConstNodeRef &node, ParserContext *context) const override;

private:
	Variant decode_from_map(const ryml::ConstNodeRef &node, ParserContext *context) const;
	Variant decode_from_sequence(const ryml::ConstNodeRef &node, ParserContext *context) const;

	std::unique_ptr<Vector3VariantConverter> vec3_converter;
};

} // namespace godot

#endif // AABB_YAML_H

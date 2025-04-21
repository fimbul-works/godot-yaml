#ifndef PACKED_VECTOR2_ARRAY_YAML_H
#define PACKED_VECTOR2_ARRAY_YAML_H

#include "../converter_factory.h"
#include "../variant_converter.h"
#include "vector2_yaml.h"

namespace godot {

class PackedVector2ArrayVariantConverter : public VariantConverter {
public:
	explicit PackedVector2ArrayVariantConverter(ConverterFactory *factory);

	DEFINE_YAML_TAG("PackedVector2Array", Variant::PACKED_VECTOR2_ARRAY)

	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;
	Variant decode(const ryml::ConstNodeRef &node) const override;

private:
	std::unique_ptr<Vector2VariantConverter> vec2_converter;
};

} // namespace godot

#endif // PACKED_VECTOR2_ARRAY_YAML_H

#ifndef PACKED_COLOR_ARRAY_YAML_H
#define PACKED_COLOR_ARRAY_H

#include "../converter_factory.h"
#include "../variant_converter.h"
#include "color_yaml.h"

namespace godot {

class PackedColorArrayVariantConverter : public VariantConverter {
public:
	explicit PackedColorArrayVariantConverter(ConverterFactory *factory);

	DEFINE_YAML_TAG("PackedColorArray", Variant::PACKED_COLOR_ARRAY)

	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;
	Variant decode(const ryml::ConstNodeRef &node) const override;

private:
	std::unique_ptr<ColorVariantConverter> color_converter;
};

} // namespace godot

#endif // PACKED_COLOR_ARRAY_YAML_H

#ifndef PACKED_FLOAT32_ARRAY_YAML_H
#define PACKED_FLOAT32_ARRAY_YAML_H

#include "../variant_converter.h"

namespace godot {

class PackedFloat32ArrayVariantConverter : public VariantConverter {
public:
	DEFINE_YAML_TAG("PackedFloat32Array", Variant::PACKED_FLOAT32_ARRAY)

	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;
	Variant decode(const ryml::ConstNodeRef &node, ParserContext *context) const override;
};

} // namespace godot

#endif // PACKED_FLOAT32_ARRAY_YAML_H

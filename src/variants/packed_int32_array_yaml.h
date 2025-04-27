#ifndef PACKED_INT32_ARRAY_YAML_H
#define PACKED_INT32_ARRAY_YAML_H

#include "../variant_converter.h"

namespace godot {

class PackedInt32ArrayVariantConverter : public VariantConverter {
public:
	DEFINE_YAML_TAG("PackedInt32Array", Variant::PACKED_INT32_ARRAY)

	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;
	Variant decode(const ryml::ConstNodeRef &node, ParserContext *context) const override;
};

} // namespace godot

#endif // PACKED_INT32_ARRAY_YAML_H

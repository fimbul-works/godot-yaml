#ifndef PACKED_FLOAT64_ARRAY_YAML_H
#define PACKED_FLOAT64_ARRAY_YAML_H

#include "../variant_converter.h"

namespace godot {

class PackedFloat64ArrayVariantConverter : public VariantConverter {
public:
	DEFINE_YAML_TAG("PackedFloat64Array", Variant::PACKED_FLOAT64_ARRAY)

	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;
	Variant decode(const ryml::ConstNodeRef &node) const override;

private:
	void emit_as_sequence(ryml::NodeRef &node, const PackedFloat64Array &array, const YAMLStyle::View &style) const;
};

} // namespace godot

#endif // PACKED_FLOAT64_ARRAY_YAML_H

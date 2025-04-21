#ifndef VECTOR4I_YAML_H
#define VECTOR4I_YAML_H

#include "../variant_converter.h"

namespace godot {

class Vector4iVariantConverter : public VariantConverter {
public:
	DEFINE_YAML_TAG("Vector4i", Variant::VECTOR4I)

	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;
	Variant decode(const ryml::ConstNodeRef &node) const override;

private:
	Variant decode_from_map(const ryml::ConstNodeRef &node) const;
	Variant decode_from_sequence(const ryml::ConstNodeRef &node) const;
};

} // namespace godot

#endif // VECTOR4I_YAML_H

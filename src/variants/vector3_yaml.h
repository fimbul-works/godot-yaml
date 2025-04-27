#ifndef VECTOR3_YAML_H
#define VECTOR3_YAML_H

#include "../variant_converter.h"

namespace godot {

class Vector3VariantConverter : public VariantConverter {
public:
	DEFINE_YAML_TAG("Vector3", Variant::VECTOR3)

	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;
	Variant decode(const ryml::ConstNodeRef &node, ParserContext *context) const override;

private:
	Vector3 decode_from_map(const ryml::ConstNodeRef &node, ParserContext *context) const;
	Vector3 decode_from_sequence(const ryml::ConstNodeRef &node, ParserContext *context) const;
};

} // namespace godot

#endif // VECTOR3_YAML_H

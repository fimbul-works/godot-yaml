#ifndef QUATERNION_YAML_H
#define QUATERNION_YAML_H

#include "../variant_converter.h"

namespace godot {

class QuaternionVariantConverter : public VariantConverter {
public:
	DEFINE_YAML_TAG("Quaternion", Variant::QUATERNION)

	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;
	Variant decode(const ryml::ConstNodeRef &node, ParserContext *context) const override;

private:
	Quaternion decode_from_map(const ryml::ConstNodeRef &node, ParserContext *context) const;
	Quaternion decode_from_sequence(const ryml::ConstNodeRef &node, ParserContext *context) const;
};

} // namespace godot

#endif // QUATERNION_YAML_H

#ifndef STRING_NAME_YAML_H
#define STRING_NAME_YAML_H

#include "../variant_converter.h"

namespace godot {

class StringNameVariantConverter : public VariantConverter {
public:
	DEFINE_YAML_TAG("StringName", Variant::STRING_NAME)

	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;
	Variant decode(const ryml::ConstNodeRef &node, ParserContext *context) const override;
};

} // namespace godot

#endif // STRING_NAME_YAML_H

#ifndef BASIS_YAML_H
#define BASIS_YAML_H

#include "../converter_factory.h"
#include "../variant_converter.h"
#include "vector3_yaml.h"

namespace godot {

class BasisVariantConverter : public VariantConverter {
public:
	explicit BasisVariantConverter(ConverterFactory *factory);

	DEFINE_YAML_TAG("Basis", Variant::BASIS)

	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;
	Variant decode(const ryml::ConstNodeRef &node) const override;

private:
	void emit_as_map(ryml::NodeRef &node, const Basis &basis, const YAMLStyle::View &style) const;
	void emit_as_sequence(ryml::NodeRef &node, const Basis &basis, const YAMLStyle::View &style) const;
	Variant decode_from_map(const ryml::ConstNodeRef &node) const;
	Variant decode_from_sequence(const ryml::ConstNodeRef &node) const;

	std::unique_ptr<Vector3VariantConverter> vec3_converter;
};

} // namespace godot

#endif // BASIS_YAML_H

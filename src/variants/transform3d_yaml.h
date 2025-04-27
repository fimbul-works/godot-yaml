#ifndef TRANSFORM3D_YAML_H
#define TRANSFORM3D_YAML_H

#include "../converter_factory.h"
#include "../variant_converter.h"
#include "basis_yaml.h"
#include "vector3_yaml.h"

namespace godot {

class Transform3DVariantConverter : public VariantConverter {
public:
	explicit Transform3DVariantConverter(ConverterFactory *factory);

	DEFINE_YAML_TAG("Transform3D", Variant::TRANSFORM3D)

	void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const override;
	Variant decode(const ryml::ConstNodeRef &node, ParserContext *context) const override;

private:
	Transform3D decode_from_map(const ryml::ConstNodeRef &node, ParserContext *context) const;
	Transform3D decode_from_sequence(const ryml::ConstNodeRef &node, ParserContext *context) const;

	std::unique_ptr<BasisVariantConverter> basis_converter;
	std::unique_ptr<Vector3VariantConverter> vec3_converter;
};

} // namespace godot

#endif // TRANSFORM3D_YAML_H

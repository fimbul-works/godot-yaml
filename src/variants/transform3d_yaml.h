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
	Variant decode(const ryml::ConstNodeRef &node) const override;

private:
	void emit_as_map(ryml::NodeRef &node, const Transform3D &transform, const YAMLStyle::View &style) const;
	void emit_as_sequence(ryml::NodeRef &node, const Transform3D &transform, const YAMLStyle::View &style) const;

	Variant decode_from_map(const ryml::ConstNodeRef &node) const;
	Variant decode_from_sequence(const ryml::ConstNodeRef &node) const;

	std::unique_ptr<BasisVariantConverter> basis_converter;
	std::unique_ptr<Vector3VariantConverter> vec3_converter;
};

} // namespace godot

#endif // TRANSFORM3D_YAML_H

#include "transform3d_yaml.h"
#include "../converter_factory.h"
#include "../exception.h"

using namespace godot;

Transform3DVariantConverter::Transform3DVariantConverter(ConverterFactory *factory) :
		basis_converter(factory->create_converter_as<BasisVariantConverter>(Variant::BASIS)),
		vec3_converter(factory->create_converter_as<Vector3VariantConverter>(Variant::VECTOR3))

{
	ERR_FAIL_NULL(basis_converter);
	ERR_FAIL_NULL(vec3_converter);
}

void Transform3DVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const Transform3D transform = v.operator Transform3D();

	if (!style.is_valid() || style.get_container_form() != YAMLStyle::FORM_SEQ) {
		emit_as_map(node, transform, style);
	} else {
		emit_as_sequence(node, transform, style);
	}
}

void Transform3DVariantConverter::emit_as_map(ryml::NodeRef &node, const Transform3D &transform, const YAMLStyle::View &style) const {
	node |= ryml::MAP;

	// Flow style
	style.apply_flow_style(node);

	// Pass child styles for basis and origin
	YAMLStyle::View basis_style = style.is_valid() ? style.get_child("basis") : YAMLStyle::View();
	YAMLStyle::View origin_style = style.is_valid() ? style.get_child("origin") : YAMLStyle::View();

	ryml::NodeRef basis_node = node["basis"];
	basis_converter->encode(basis_node, transform.basis, basis_style);

	ryml::NodeRef origin_node = node["origin"];
	vec3_converter->encode(origin_node, transform.origin, origin_style);
}

void Transform3DVariantConverter::emit_as_sequence(ryml::NodeRef &node, const Transform3D &transform, const YAMLStyle::View &style) const {
	node |= ryml::SEQ;

	// Flow style
	style.apply_flow_style(node);

	// Pass child styles for each vector using indices
	for (int i = 0; i < 3; i++) {
		const YAMLStyle::View &row_style = style.is_valid() ? style.get_child(String::num_int64(i)) : YAMLStyle::View();
		ryml::NodeRef col_node = node.append_child();
		vec3_converter->encode(col_node, transform.basis.rows[i], row_style);
	}

	// Origin gets index 3
	YAMLStyle::View origin_style = style.is_valid() ? style.get_child("3") : YAMLStyle::View();
	ryml::NodeRef origin_node = node.append_child();
	vec3_converter->encode(origin_node, transform.origin, origin_style);
}

Variant Transform3DVariantConverter::decode(const ryml::ConstNodeRef &node) const {
	try {
		if (node.is_map()) {
			return decode_from_map(node);
		} else if (node.is_seq()) {
			return decode_from_sequence(node);
		}
		throw YAMLException::create_invalid_format("Transform3D");
	} catch (const YAMLException &) {
		throw;
	} catch (const std::exception &e) {
		throw YAMLException::create_decode_error("Transform3D", e.what());
	}
}

Variant Transform3DVariantConverter::decode_from_map(const ryml::ConstNodeRef &node) const {
	if (!node.has_child("basis") || !node.has_child("origin")) {
		throw YAMLException::create_missing_field("Transform3D", "basis, origin");
	}

	Basis basis = basis_converter->decode(node["basis"]).operator Basis();
	Vector3 origin = vec3_converter->decode(node["origin"]).operator Vector3();

	return Transform3D(basis, origin);
}

Variant Transform3DVariantConverter::decode_from_sequence(const ryml::ConstNodeRef &node) const {
	if (node.num_children() != 4) {
		throw YAMLException::create_invalid_sequence_length("Transform3D", 4);
	}

	Basis basis;

	// Read basis columns
	for (int i = 0; i < 3; i++) {
		basis.rows[i] = vec3_converter->decode(node[i]).operator Vector3();
	}

	// Read origin
	Vector3 origin = vec3_converter->decode(node[3]).operator Vector3();

	return Transform3D(basis, origin);
}

#include "transform3d_yaml.h"
#include "../converter_factory.h"
#include "../exception.h"

using namespace godot;

Transform3DVariantConverter::Transform3DVariantConverter(ConverterFactory *factory) :
		basis_converter(factory->create_converter_as<BasisVariantConverter>(Variant::BASIS)),
		vec3_converter(factory->create_converter_as<Vector3VariantConverter>(Variant::VECTOR3)) {
	ERR_FAIL_NULL(basis_converter);
	ERR_FAIL_NULL(vec3_converter);
}

void Transform3DVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const Transform3D transform = v.operator Transform3D();

	style.apply_flow_style(node);

	if (!style.is_valid() || style.get_container_form() != YAMLStyle::FORM_SEQ) {
		node |= ryml::MAP;
		basis_converter->encode(node["basis"], transform.basis, style.get_child("basis"));
		vec3_converter->encode(node["origin"], transform.origin, style.get_child("origin"));
	} else {
		node |= ryml::SEQ;
		basis_converter->encode(node.append_child(), transform.basis, style.get_child("basis"));
		vec3_converter->encode(node.append_child(), transform.origin, style.get_child("origin"));
	}
}

Variant Transform3DVariantConverter::decode(const ryml::ConstNodeRef &node, ParserContext *context) const {
	try {
		if (node.is_map()) {
			return decode_from_map(node, context);
		}

		if (node.is_seq()) {
			return decode_from_sequence(node, context);
		}

		throw create_invalid_format_exception(node);
	} catch (const YAMLException &e) {
		throw YAMLException(vformat("Failed to decode Transform3D: %s", e.what()), e.get_location());
	} catch (const std::exception &e) {
		throw create_decode_error_exception(e.what(), node);
	}
}

Transform3D Transform3DVariantConverter::decode_from_map(const ryml::ConstNodeRef &node, ParserContext *context) const {
	check_required_fields(node, { "basis", "origin" });

	const bool detect_style = context->detect_style;

	if (detect_style) {
		Ref<YAMLStyle> style = context->current_style();
		YAMLStyle::detect_flow_style(node, style);
		style->set_container_form(YAMLStyle::FORM_MAP);

		context->push_style("basis");
	}

	Basis basis = basis_converter->decode(node["basis"], context).operator Basis();

	if (detect_style) {
		context->pop_style();
		context->push_style("origin");
	}

	Vector3 origin = vec3_converter->decode(node["origin"], context).operator Vector3();

	if (detect_style) {
		context->pop_style();
	}

	return Transform3D(basis, origin);
}

Transform3D Transform3DVariantConverter::decode_from_sequence(const ryml::ConstNodeRef &node, ParserContext *context) const {
	if (node.num_children() != 4) {
		throw create_invalid_sequence_length_exception(4, node);
	}

	const bool detect_style = context->detect_style;

	if (detect_style) {
		Ref<YAMLStyle> style = context->current_style();
		YAMLStyle::detect_flow_style(node, style);
		style->set_container_form(YAMLStyle::FORM_SEQ);

		context->push_style("basis");
	}

	Basis basis = basis_converter->decode(node["basis"], context).operator Basis();

	if (detect_style) {
		context->pop_style();
		context->push_style("origin");
	}

	Vector3 origin = vec3_converter->decode(node[3], context).operator Vector3();

	if (detect_style) {
		context->pop_style();
	}

	return Transform3D(basis, origin);
}

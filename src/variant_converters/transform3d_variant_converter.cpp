#include "transform3d_variant_converter.hpp"
#include "../exception.hpp"
#include "variant_converter_factory.hpp"

using namespace godot;

YAMLTransform3DVariantConverter::YAMLTransform3DVariantConverter(YAMLVariantConverterFactory *factory) :
		basis_converter(factory->create_converter_as<YAMLBasisVariantConverter>(Variant::BASIS)),
		vec3_converter(factory->create_converter_as<YAMLVector3VariantConverter>(Variant::VECTOR3)) {
	ERR_FAIL_NULL(basis_converter);
	ERR_FAIL_NULL(vec3_converter);
}

void YAMLTransform3DVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const Transform3D transform = v.operator Transform3D();

	style.apply_flow_style(node);

	ryml::NodeRef basis_node;
	ryml::NodeRef origin_node;

	if (!style.is_valid() || style.get_container_form() != YAMLStyle::FORM_ARRAY) {
		node |= ryml::MAP;
		basis_node = node["basis"];
		origin_node = node["origin"];
	} else {
		node |= ryml::SEQ;
		basis_node = node.append_child();
		origin_node = node.append_child();
	}

	auto scalar_style = style.get_scalar_view();
	basis_converter->encode(basis_node, transform.basis, style.has_child("basis") ? style.get_child("basis") : scalar_style);
	vec3_converter->encode(origin_node, transform.origin, style.has_child("origin") ? style.get_child("origin") : scalar_style);
}

Variant YAMLTransform3DVariantConverter::decode(const ryml::ConstNodeRef &node, YAMLParserContext *context) const {
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

Transform3D YAMLTransform3DVariantConverter::decode_from_map(const ryml::ConstNodeRef &node, YAMLParserContext *context) const {
	check_required_fields(node, { "basis", "origin" });

	const bool detect_style = context->detect_style;

	if (detect_style) {
		Ref<YAMLStyle> style = context->current_style();
		YAMLStyle::detect_flow_style(node, style);
		style->set_container_form(YAMLStyle::FORM_DICTIONARY);

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

Transform3D YAMLTransform3DVariantConverter::decode_from_sequence(const ryml::ConstNodeRef &node, YAMLParserContext *context) const {
	if (node.num_children() != 4) {
		throw create_invalid_sequence_length_exception(4, node);
	}

	const bool detect_style = context->detect_style;

	if (detect_style) {
		Ref<YAMLStyle> style = context->current_style();
		YAMLStyle::detect_flow_style(node, style);
		style->set_container_form(YAMLStyle::FORM_ARRAY);

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

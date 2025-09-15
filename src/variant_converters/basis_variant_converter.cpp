#include "basis_variant_converter.hpp"
#include "../exception.hpp"
#include "variant_converter_factory.hpp"
#include "vector3_variant_converter.hpp"

using namespace godot;

YAMLBasisVariantConverter::YAMLBasisVariantConverter(YAMLVariantConverterFactory *factory) :
		vec3_converter(factory->create_converter_as<YAMLVector3VariantConverter>(Variant::VECTOR3)) {
	ERR_FAIL_NULL(vec3_converter);
}

void YAMLBasisVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const Basis basis = v.operator Basis();

	style.apply_flow_style(node);

	ryml::NodeRef x_node;
	ryml::NodeRef y_node;
	ryml::NodeRef z_node;

	if (!style.is_valid() || style.get_container_form() != YAMLStyle::FORM_ARRAY) {
		node |= ryml::MAP;
		x_node = node["x"];
		y_node = node["y"];
		z_node = node["z"];
	} else {
		node |= ryml::SEQ;
		x_node = node.append_child();
		y_node = node.append_child();
		z_node = node.append_child();
	}

	auto scalar_style = style.get_scalar_view();
	vec3_converter->encode(x_node, basis.get_column(0), style.has_child("x") ? style.get_child("x") : scalar_style);
	vec3_converter->encode(y_node, basis.get_column(1), style.has_child("y") ? style.get_child("y") : scalar_style);
	vec3_converter->encode(z_node, basis.get_column(2), style.has_child("z") ? style.get_child("z") : scalar_style);
}

Variant YAMLBasisVariantConverter::decode(const ryml::ConstNodeRef &node, YAMLParserContext *context) const {
	try {
		if (node.is_map()) {
			return decode_from_map(node, context);
		}

		if (node.is_seq()) {
			return decode_from_sequence(node, context);
		}

		throw create_invalid_format_exception(node);
	} catch (const YAMLException &) {
		throw; // Re-throw YAML exceptions
	} catch (const std::exception &e) {
		throw create_decode_error_exception(e.what(), node);
	}
}

Variant YAMLBasisVariantConverter::decode_from_map(const ryml::ConstNodeRef &node, YAMLParserContext *context) const {
	check_required_fields(node, { "x", "y", "z" });

	const bool detect_style = context->detect_style;

	if (detect_style) {
		Ref<YAMLStyle> style = context->current_style();
		YAMLStyle::detect_flow_style(node, style);
		style->set_container_form(YAMLStyle::FORM_DICTIONARY);

		context->push_style("x");
	}

	Vector3 x = vec3_converter->decode(node["x"], context).operator Vector3();

	if (detect_style) {
		context->pop_style();
		context->push_style("y");
	}

	Vector3 y = vec3_converter->decode(node["y"], context).operator Vector3();

	if (detect_style) {
		context->pop_style();
		context->push_style("z");
	}

	Vector3 z = vec3_converter->decode(node["z"], context).operator Vector3();

	if (detect_style) {
		context->pop_style();
	}

	return Basis(x, y, z);
}

Variant YAMLBasisVariantConverter::decode_from_sequence(const ryml::ConstNodeRef &node, YAMLParserContext *context) const {
	if (node.num_children() != 3) {
		throw create_invalid_sequence_length_exception(3, node);
	}

	const bool detect_style = context->detect_style;

	if (detect_style) {
		Ref<YAMLStyle> style = context->current_style();
		YAMLStyle::detect_flow_style(node, style);
		style->set_container_form(YAMLStyle::FORM_ARRAY);

		context->push_style("x");
	}

	Vector3 x = vec3_converter->decode(node[0], context).operator Vector3();

	if (detect_style) {
		context->pop_style();
		context->push_style("y");
	}

	Vector3 y = vec3_converter->decode(node[1], context).operator Vector3();

	if (detect_style) {
		context->pop_style();
		context->push_style("z");
	}

	Vector3 z = vec3_converter->decode(node[2], context).operator Vector3();

	if (detect_style) {
		context->pop_style();
	}

	return Basis(x, y, z);
}

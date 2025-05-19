#include "basis_yaml.h"
#include "../converter_factory.h"
#include "../exception.h"
#include "vector3_yaml.h"

using namespace godot;

BasisVariantConverter::BasisVariantConverter(ConverterFactory *factory) :
		vec3_converter(factory->create_converter_as<Vector3VariantConverter>(Variant::VECTOR3)) {
	ERR_FAIL_NULL(vec3_converter);
}

void BasisVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
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

	vec3_converter->encode(x_node, basis.get_column(0), style.has_child("x") ? style.get_child("x") : YAMLStyle::View());
	vec3_converter->encode(y_node, basis.get_column(1), style.has_child("y") ? style.get_child("y") : YAMLStyle::View());
	vec3_converter->encode(z_node, basis.get_column(2), style.has_child("z") ? style.get_child("z") : YAMLStyle::View());
}

Variant BasisVariantConverter::decode(const ryml::ConstNodeRef &node, ParserContext *context) const {
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

Variant BasisVariantConverter::decode_from_map(const ryml::ConstNodeRef &node, ParserContext *context) const {
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

Variant BasisVariantConverter::decode_from_sequence(const ryml::ConstNodeRef &node, ParserContext *context) const {
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

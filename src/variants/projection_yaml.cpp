#include "projection_yaml.h"
#include "../converter_factory.h"
#include "../exception.h"
#include "../util_numeric.h"

using namespace godot;

ProjectionVariantConverter::ProjectionVariantConverter(ConverterFactory *factory) :
		vec4_converter(factory->create_converter_as<Vector4VariantConverter>(Variant::VECTOR4)) {
	ERR_FAIL_NULL(vec4_converter);
}

void ProjectionVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const Projection proj = v.operator Projection();

	style.apply_flow_style(node);

	YAMLStyle::View x_style = style.is_valid() ? style.get_child("x") : YAMLStyle::View();
	YAMLStyle::View y_style = style.is_valid() ? style.get_child("y") : YAMLStyle::View();
	YAMLStyle::View z_style = style.is_valid() ? style.get_child("z") : YAMLStyle::View();
	YAMLStyle::View w_style = style.is_valid() ? style.get_child("w") : YAMLStyle::View();

	if (!style.is_valid() || style.get_container_form() != YAMLStyle::FORM_SEQ) {
		node |= ryml::MAP;
		vec4_converter->encode(node["x"], proj.columns[0], x_style);
		vec4_converter->encode(node["y"], proj.columns[1], y_style);
		vec4_converter->encode(node["z"], proj.columns[2], z_style);
		vec4_converter->encode(node["w"], proj.columns[3], w_style);
	} else {
		node |= ryml::SEQ;
		vec4_converter->encode(node.append_child(), proj.columns[0], x_style);
		vec4_converter->encode(node.append_child(), proj.columns[1], y_style);
		vec4_converter->encode(node.append_child(), proj.columns[2], z_style);
		vec4_converter->encode(node.append_child(), proj.columns[3], w_style);
	}
}

Variant ProjectionVariantConverter::decode(const ryml::ConstNodeRef &node, ParserContext *context) const {
	try {
		if (node.is_map()) {
			return decode_from_map(node, context);
		}

		if (node.is_seq()) {
			return decode_from_sequence(node, context);
		}

		throw create_invalid_format_exception(node);
	} catch (const YAMLException &e) {
		throw YAMLException(vformat("Failed to decode Projection: %s", e.what()), e.get_location());
	} catch (const std::exception &e) {
		throw create_decode_error_exception(e.what(), node);
	}
}

Variant ProjectionVariantConverter::decode_from_map(const ryml::ConstNodeRef &node, ParserContext *context) const {
	check_required_fields(node, { "x", "y", "z", "w" });

	Projection proj;

	const bool detect_style = context->detect_style;

	if (detect_style) {
		Ref<YAMLStyle> style = context->current_style();
		YAMLStyle::detect_flow_style(node, style);
		style->set_container_form(YAMLStyle::FORM_MAP);

		context->push_style("x");
	}

	proj.columns[0] = vec4_converter->decode(node["x"], context).operator Vector4();

	if (detect_style) {
		context->pop_style();
		context->push_style("y");
	}

	proj.columns[1] = vec4_converter->decode(node["y"], context).operator Vector4();

	if (detect_style) {
		context->pop_style();
		context->push_style("z");
	}

	proj.columns[2] = vec4_converter->decode(node["z"], context).operator Vector4();

	if (detect_style) {
		context->pop_style();
		context->push_style("w");
	}

	proj.columns[3] = vec4_converter->decode(node["w"], context).operator Vector4();

	if (detect_style) {
		context->pop_style();
	}

	return proj;
}

Variant ProjectionVariantConverter::decode_from_sequence(const ryml::ConstNodeRef &node, ParserContext *context) const {
	if (node.num_children() != 4) {
		throw create_invalid_sequence_length_exception(4, node);
	}

	Projection proj;

	const bool detect_style = context->detect_style;

	if (detect_style) {
		Ref<YAMLStyle> style = context->current_style();
		YAMLStyle::detect_flow_style(node, style);
		style->set_container_form(YAMLStyle::FORM_SEQ);

		context->push_style("x");
	}

	proj.columns[0] = vec4_converter->decode(node[0], context).operator Vector4();

	if (detect_style) {
		context->pop_style();
		context->push_style("y");
	}

	proj.columns[1] = vec4_converter->decode(node[1], context).operator Vector4();

	if (detect_style) {
		context->pop_style();
		context->push_style("z");
	}

	proj.columns[2] = vec4_converter->decode(node[2], context).operator Vector4();

	if (detect_style) {
		context->pop_style();
		context->push_style("w");
	}

	proj.columns[3] = vec4_converter->decode(node[3], context).operator Vector4();

	if (detect_style) {
		context->pop_style();
	}

	return proj;
}

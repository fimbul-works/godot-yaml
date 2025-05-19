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

	ryml::NodeRef x_node;
	ryml::NodeRef y_node;
	ryml::NodeRef z_node;
	ryml::NodeRef w_node;

	if (!style.is_valid() || style.get_container_form() != YAMLStyle::FORM_ARRAY) {
		node |= ryml::MAP;
		x_node = node["x"];
		y_node = node["y"];
		z_node = node["z"];
		w_node = node["w"];
	} else {
		node |= ryml::SEQ;
		x_node = node.append_child();
		y_node = node.append_child();
		z_node = node.append_child();
		w_node = node.append_child();
	}

	vec4_converter->encode(x_node, proj.columns[0], style.has_child("x") ? style.get_child("x") : YAMLStyle::View());
	vec4_converter->encode(y_node, proj.columns[1], style.has_child("y") ? style.get_child("y") : YAMLStyle::View());
	vec4_converter->encode(z_node, proj.columns[2], style.has_child("z") ? style.get_child("z") : YAMLStyle::View());
	vec4_converter->encode(w_node, proj.columns[3], style.has_child("w") ? style.get_child("w") : YAMLStyle::View());
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
		style->set_container_form(YAMLStyle::FORM_DICTIONARY);

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
		style->set_container_form(YAMLStyle::FORM_ARRAY);

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

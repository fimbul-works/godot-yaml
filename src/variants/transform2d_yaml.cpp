#include "transform2d_yaml.h"
#include "../converter_factory.h"
#include "../exception.h"

using namespace godot;

Transform2DVariantConverter::Transform2DVariantConverter(ConverterFactory *factory) :
		vec2_converter(factory->create_converter_as<Vector2VariantConverter>(Variant::VECTOR2)) {
	ERR_FAIL_NULL(vec2_converter);
}

void Transform2DVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const Transform2D transform = v.operator Transform2D();

	style.apply_flow_style(node);

	ryml::NodeRef x_node;
	ryml::NodeRef y_node;
	ryml::NodeRef origin_node;

	if (!style.is_valid() || style.get_container_form() != YAMLStyle::FORM_ARRAY) {
		node |= ryml::MAP;
		x_node = node["x"];
		y_node = node["y"];
		origin_node = node["origin"];
	} else {
		node |= ryml::SEQ;
		x_node = node.append_child();
		y_node = node.append_child();
		origin_node = node.append_child();
	}

	auto scalar_style = style.get_scalar_view();
	vec2_converter->encode(x_node, transform.columns[0], style.has_child("x") ? style.get_child("x") : scalar_style);
	vec2_converter->encode(y_node, transform.columns[1], style.has_child("y") ? style.get_child("y") : scalar_style);
	vec2_converter->encode(origin_node, transform.columns[2], style.has_child("origin") ? style.get_child("origin") : scalar_style);
}

Variant Transform2DVariantConverter::decode(const ryml::ConstNodeRef &node, ParserContext *context) const {
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

Transform2D Transform2DVariantConverter::decode_from_map(const ryml::ConstNodeRef &node, ParserContext *context) const {
	check_required_fields(node, { "x", "y", "origin" });

	const bool detect_style = context->detect_style;

	if (detect_style) {
		Ref<YAMLStyle> style = context->current_style();
		YAMLStyle::detect_flow_style(node, style);
		style->set_container_form(YAMLStyle::FORM_DICTIONARY);

		context->push_style("x");
	}

	Vector2 x = vec2_converter->decode(node["x"], context).operator Vector2();

	if (detect_style) {
		context->pop_style();
		context->push_style("y");
	}

	Vector2 y = vec2_converter->decode(node["y"], context).operator Vector2();

	if (detect_style) {
		context->pop_style();
		context->push_style("origin");
	}

	Vector2 origin = vec2_converter->decode(node["origin"], context).operator Vector2();

	if (detect_style) {
		context->pop_style();
	}

	return Transform2D(x, y, origin);
}

Transform2D Transform2DVariantConverter::decode_from_sequence(const ryml::ConstNodeRef &node, ParserContext *context) const {
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

	Vector2 x = vec2_converter->decode(node[0], context).operator Vector2();

	if (detect_style) {
		context->pop_style();
		context->push_style("y");
	}

	Vector2 y = vec2_converter->decode(node[1], context).operator Vector2();

	if (detect_style) {
		context->pop_style();
		context->push_style("origin");
	}

	Vector2 origin = vec2_converter->decode(node[2], context).operator Vector2();

	if (detect_style) {
		context->pop_style();
	}

	return Transform2D(x, y, origin);
}

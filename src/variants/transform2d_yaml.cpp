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

	if (!style.is_valid() || style.get_container_form() != YAMLStyle::FORM_SEQ) {
		emit_as_map(node, transform, style);
	} else {
		emit_as_sequence(node, transform, style);
	}
}

void Transform2DVariantConverter::emit_as_map(ryml::NodeRef &node, const Transform2D &transform, const YAMLStyle::View &style) const {
	node |= ryml::MAP;

	style.apply_flow_style(node);

	YAMLStyle::View x_style = style.is_valid() ? style.get_child("x") : YAMLStyle::View();
	YAMLStyle::View y_style = style.is_valid() ? style.get_child("y") : YAMLStyle::View();
	YAMLStyle::View origin_style = style.is_valid() ? style.get_child("origin") : YAMLStyle::View();

	ryml::NodeRef x_node = node["x"];
	vec2_converter->encode(x_node, transform.columns[0], x_style);

	ryml::NodeRef y_node = node["y"];
	vec2_converter->encode(y_node, transform.columns[1], y_style);

	ryml::NodeRef origin_node = node["origin"];
	vec2_converter->encode(origin_node, transform.columns[2], origin_style);
}

void Transform2DVariantConverter::emit_as_sequence(ryml::NodeRef &node, const Transform2D &transform, const YAMLStyle::View &style) const {
	node |= ryml::SEQ;

	style.apply_flow_style(node);

	YAMLStyle::View x_style = style.is_valid() ? style.get_child("x") : YAMLStyle::View();
	YAMLStyle::View y_style = style.is_valid() ? style.get_child("y") : YAMLStyle::View();
	YAMLStyle::View origin_style = style.is_valid() ? style.get_child("origin") : YAMLStyle::View();

	ryml::NodeRef x_node = node.append_child();
	vec2_converter->encode(x_node, transform.columns[0], x_style);

	ryml::NodeRef y_node = node.append_child();
	vec2_converter->encode(y_node, transform.columns[1], y_style);

	ryml::NodeRef origin_node = node.append_child();
	vec2_converter->encode(origin_node, transform.columns[2], origin_style);
}

Variant Transform2DVariantConverter::decode(const ryml::ConstNodeRef &node) const {
	try {
		if (node.is_map()) {
			return decode_from_map(node);
		}

		if (node.is_seq()) {
			return decode_from_sequence(node);
		}

		throw create_invalid_format_exception("Transform2D", node);
	} catch (const YAMLException &) {
		throw; // Re-throw YAML exceptions
	} catch (const std::exception &e) {
		throw create_decode_error_exception("Transform2D", e.what(), node);
	}
}

Variant Transform2DVariantConverter::decode_from_map(const ryml::ConstNodeRef &node) const {
	check_required_fields(node, { "x", "y", "origin" });

	Vector2 x = vec2_converter->decode(node["x"]).operator Vector2();
	Vector2 y = vec2_converter->decode(node["y"]).operator Vector2();
	Vector2 origin = vec2_converter->decode(node["origin"]).operator Vector2();

	return Transform2D(x, y, origin);
}

Variant Transform2DVariantConverter::decode_from_sequence(const ryml::ConstNodeRef &node) const {
	if (node.num_children() != 3) {
		throw create_invalid_sequence_length_exception("Transform2D", 3, node);
	}

	Vector2 x = vec2_converter->decode(node[0]).operator Vector2();
	Vector2 y = vec2_converter->decode(node[1]).operator Vector2();
	Vector2 origin = vec2_converter->decode(node[2]).operator Vector2();

	return Transform2D(x, y, origin);
}

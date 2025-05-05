#include "vector3i_yaml.h"
#include "../exception.h"
#include "../util_numeric.h"

using namespace godot;

void Vector3iVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const Vector3i vec = v.operator Vector3i();

	if (style.is_valid()) {
		style.apply_flow_style(node);
	} else {
		node |= ryml::FLOW_SL;
	}

	YAMLStyle::IntegerFormat x_format = style.has_child("x") ? style.get_child("x").get_integer_format() : style.get_integer_format();
	YAMLStyle::IntegerFormat y_format = style.has_child("y") ? style.get_child("y").get_integer_format() : style.get_integer_format();
	YAMLStyle::IntegerFormat z_format = style.has_child("z") ? style.get_child("z").get_integer_format() : style.get_integer_format();

	if (!style.is_valid() || style.get_container_form() != YAMLStyle::FORM_ARRAY) {
		node |= ryml::MAP;

		node["x"] << int_to_string(vec.x, x_format);
		node["y"] << int_to_string(vec.y, y_format);
		node["z"] << int_to_string(vec.z, z_format);
	} else {
		node |= ryml::SEQ;

		node.append_child() << int_to_string(vec.x, x_format);
		node.append_child() << int_to_string(vec.y, y_format);
		node.append_child() << int_to_string(vec.z, z_format);
	}
}

Variant Vector3iVariantConverter::decode(const ryml::ConstNodeRef &node, ParserContext *context) const {
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

Vector3i Vector3iVariantConverter::decode_from_map(const ryml::ConstNodeRef &node, ParserContext *context) const {
	check_required_fields(node, { "x", "y", "z" });

	const bool detect_style = context->detect_style;

	if (detect_style) {
		Ref<YAMLStyle> style = context->current_style();
		YAMLStyle::detect_flow_style(node, style);
		style->set_container_form(YAMLStyle::FORM_DICTIONARY);

		context->push_style("x");
	}

	YAMLStyle::IntegerFormat x_format;
	int32_t x = string_to_int<int32_t>(node[0].val(), detect_style ? &x_format : nullptr);

	if (detect_style) {
		context->current_style()->set_integer_format(x_format);
		context->pop_style();

		context->push_style("y");
	}

	YAMLStyle::IntegerFormat y_format;
	int32_t y = string_to_int<int32_t>(node["y"].val(), detect_style ? &y_format : nullptr);

	if (detect_style) {
		context->current_style()->set_integer_format(y_format);
		context->pop_style();

		context->push_style("z");
	}

	YAMLStyle::IntegerFormat z_format;
	int32_t z = string_to_int<int32_t>(node["z"].val(), detect_style ? &z_format : nullptr);

	if (context->detect_style) {
		context->current_style()->set_integer_format(z_format);
		context->pop_style();
	}

	return Vector3i(x, y, z);
}

Vector3i Vector3iVariantConverter::decode_from_sequence(const ryml::ConstNodeRef &node, ParserContext *context) const {
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

	YAMLStyle::IntegerFormat x_format;
	int32_t x = string_to_int<int32_t>(node[0].val(), detect_style ? &x_format : nullptr);

	if (detect_style) {
		context->current_style()->set_integer_format(x_format);
		context->pop_style();

		context->push_style("y");
	}

	YAMLStyle::IntegerFormat y_format;
	int32_t y = string_to_int<int32_t>(node[1].val(), detect_style ? &y_format : nullptr);

	if (detect_style) {
		context->current_style()->set_integer_format(y_format);
		context->pop_style();

		context->push_style("z");
	}

	YAMLStyle::IntegerFormat z_format;
	int32_t z = string_to_int<int32_t>(node[2].val(), detect_style ? &z_format : nullptr);

	if (detect_style) {
		context->current_style()->set_integer_format(z_format);
		context->pop_style();
	}

	return Vector3i(x, y, z);
}

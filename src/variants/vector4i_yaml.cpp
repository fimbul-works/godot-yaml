#include "vector4i_yaml.h"
#include "../exception.h"
#include "../util_numeric.h"

using namespace godot;

void Vector4iVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const Vector4i vec = v.operator Vector4i();

	style.apply_flow_style(node);

	YAMLStyle::IntegerFormat x_format = style.has_child("x") ? style.get_child("x").get_integer_format() : style.get_integer_format();
	YAMLStyle::IntegerFormat y_format = style.has_child("y") ? style.get_child("y").get_integer_format() : style.get_integer_format();
	YAMLStyle::IntegerFormat z_format = style.has_child("z") ? style.get_child("z").get_integer_format() : style.get_integer_format();
	YAMLStyle::IntegerFormat w_format = style.has_child("w") ? style.get_child("w").get_integer_format() : style.get_integer_format();

	if (!style.is_valid() || style.get_container_form() != YAMLStyle::FORM_SEQ) {
		node |= ryml::MAP;

		node["x"] << int_to_string(vec.x, x_format);
		node["y"] << int_to_string(vec.y, y_format);
		node["z"] << int_to_string(vec.z, z_format);
		node["w"] << int_to_string(vec.w, w_format);
	} else {
		node |= ryml::SEQ;

		node.append_child() << int_to_string(vec.x, x_format);
		node.append_child() << int_to_string(vec.y, y_format);
		node.append_child() << int_to_string(vec.z, z_format);
		node.append_child() << int_to_string(vec.w, w_format);
	}
}

Variant Vector4iVariantConverter::decode(const ryml::ConstNodeRef &node, ParserContext *context) const {
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

Vector4i Vector4iVariantConverter::decode_from_map(const ryml::ConstNodeRef &node, ParserContext *context) const {
	check_required_fields(node, { "x", "y", "z", "w" });

	const bool detect_style = context->detect_style;

	if (detect_style) {
		Ref<YAMLStyle> style = context->current_style();
		YAMLStyle::detect_flow_style(node, style);
		style->set_container_form(YAMLStyle::FORM_MAP);

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

	if (detect_style) {
		context->current_style()->set_integer_format(z_format);
		context->pop_style();

		context->push_style("w");
	}

	YAMLStyle::IntegerFormat w_format;
	int32_t w = string_to_int<int32_t>(node["w"].val(), detect_style ? &w_format : nullptr);

	if (detect_style) {
		context->current_style()->set_integer_format(w_format);
		context->pop_style();
	}

	return Vector4i(x, y, z, w);
}

Vector4i Vector4iVariantConverter::decode_from_sequence(const ryml::ConstNodeRef &node, ParserContext *context) const {
	if (node.num_children() != 4) {
		throw create_invalid_sequence_length_exception(4, node);
	}

	const bool detect_style = context->detect_style;

	if (detect_style) {
		Ref<YAMLStyle> style = context->current_style();
		YAMLStyle::detect_flow_style(node, style);
		style->set_container_form(YAMLStyle::FORM_SEQ);

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

		context->push_style("w");
	}

	YAMLStyle::IntegerFormat w_format;
	int32_t w = string_to_int<int32_t>(node[3].val(), detect_style ? &w_format : nullptr);

	if (detect_style) {
		context->current_style()->set_integer_format(w_format);
		context->pop_style();
	}

	return Vector4i(x, y, z, w);
}

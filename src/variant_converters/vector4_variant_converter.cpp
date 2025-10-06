#include "vector4_variant_converter.hpp"
#include "../exception.hpp"
#include "../util/util_numeric.hpp"

using namespace godot;

void YAMLVector4VariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const Vector4 vec = v.operator Vector4();

	if (style.is_valid()) {
		style.apply_flow_style(node);
	} else {
		node |= ryml::FLOW_SL;
	}

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

	x_node << float_to_string(vec.x, style.has_child("x") ? style.get_child("x").get_float_format() : style.get_float_format());
	y_node << float_to_string(vec.y, style.has_child("y") ? style.get_child("y").get_float_format() : style.get_float_format());
	z_node << float_to_string(vec.z, style.has_child("z") ? style.get_child("z").get_float_format() : style.get_float_format());
	w_node << float_to_string(vec.w, style.has_child("w") ? style.get_child("w").get_float_format() : style.get_float_format());
}

Variant YAMLVector4VariantConverter::decode(const ryml::ConstNodeRef &node, YAMLParserContext *context) const {
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

Vector4 YAMLVector4VariantConverter::decode_from_map(const ryml::ConstNodeRef &node, YAMLParserContext *context) const {
	check_required_fields(node, { "x", "y", "z", "w" });

	const bool detect_style = context->detect_style;

	if (detect_style) {
		Ref<YAMLStyle> style = context->current_style();
		YAMLStyle::detect_flow_style(node, style);
		style->set_container_form(YAMLStyle::FORM_DICTIONARY);

		context->push_style("x");
	}

	YAMLStyle::FloatFormat x_format;
	real_t x = string_to_float<real_t>(node[0].val(), detect_style ? &x_format : nullptr);

	if (detect_style) {
		context->current_style()->set_float_format(x_format);
		context->pop_style();

		context->push_style("y");
	}

	YAMLStyle::FloatFormat y_format;
	real_t y = string_to_float<real_t>(node["y"].val(), detect_style ? &y_format : nullptr);

	if (detect_style) {
		context->current_style()->set_float_format(y_format);
		context->pop_style();

		context->push_style("z");
	}

	YAMLStyle::FloatFormat z_format;
	real_t z = string_to_float<real_t>(node["z"].val(), detect_style ? &z_format : nullptr);

	if (detect_style) {
		context->current_style()->set_float_format(z_format);
		context->pop_style();

		context->push_style("w");
	}

	YAMLStyle::FloatFormat w_format;
	real_t w = string_to_float<real_t>(node["w"].val(), detect_style ? &w_format : nullptr);

	if (detect_style) {
		context->current_style()->set_float_format(w_format);
		context->pop_style();
	}

	return Vector4i(x, y, z, w);
}

Vector4 YAMLVector4VariantConverter::decode_from_sequence(const ryml::ConstNodeRef &node, YAMLParserContext *context) const {
	if (node.num_children() != 4) {
		throw create_invalid_sequence_length_exception(4, node);
	}

	const bool detect_style = context->detect_style;

	if (detect_style) {
		Ref<YAMLStyle> style = context->current_style();
		YAMLStyle::detect_flow_style(node, style);
		style->set_container_form(YAMLStyle::FORM_ARRAY);

		context->push_style("x");
	}

	YAMLStyle::FloatFormat x_format;
	real_t x = string_to_float<real_t>(node[0].val(), detect_style ? &x_format : nullptr);

	if (detect_style) {
		context->current_style()->set_float_format(x_format);
		context->pop_style();

		context->push_style("y");
	}

	YAMLStyle::FloatFormat y_format;
	real_t y = string_to_float<real_t>(node[1].val(), detect_style ? &y_format : nullptr);

	if (detect_style) {
		context->current_style()->set_float_format(y_format);
		context->pop_style();

		context->push_style("z");
	}

	YAMLStyle::FloatFormat z_format;
	real_t z = string_to_float<real_t>(node[2].val(), detect_style ? &z_format : nullptr);

	if (context->detect_style) {
		context->current_style()->set_float_format(z_format);
		context->pop_style();

		context->push_style("w");
	}

	YAMLStyle::FloatFormat w_format;
	real_t w = string_to_float<real_t>(node[3].val(), detect_style ? &w_format : nullptr);

	if (context->detect_style) {
		context->current_style()->set_float_format(w_format);
		context->pop_style();
	}

	return Vector4(x, y, z, w);
}

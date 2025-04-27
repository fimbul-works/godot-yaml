#include "color_yaml.h"
#include "../exception.h"
#include "../util_numeric.h"

using namespace godot;

void ColorVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const Color color = v.operator Color();

	if (style.get_container_form() == YAMLStyle::FORM_MAP) {
		emit_as_map(node, color, style);
	} else if (style.get_container_form() == YAMLStyle::FORM_SEQ) {
		emit_as_sequence(node, color, style);
	} else {
		node << store_string(color.to_html(color.a < 1.0f));
	}
}

void ColorVariantConverter::emit_as_map(ryml::NodeRef &node, const Color &color, const YAMLStyle::View &style) const {
	node |= ryml::MAP;

	style.apply_flow_style(node);

	YAMLStyle::FloatFormat r_format = style.has_child("r") ? style.get_child("r").get_float_format() : style.get_float_format();
	YAMLStyle::FloatFormat g_format = style.has_child("g") ? style.get_child("g").get_float_format() : style.get_float_format();
	YAMLStyle::FloatFormat b_format = style.has_child("b") ? style.get_child("b").get_float_format() : style.get_float_format();

	node["r"] << float_to_string(color.r, r_format);
	node["g"] << float_to_string(color.g, g_format);
	node["b"] << float_to_string(color.b, b_format);

	if (color.a < 1.0f) {
		YAMLStyle::FloatFormat a_format = style.has_child("a") ? style.get_child("a").get_float_format() : style.get_float_format();
		node["a"] << float_to_string(color.a, a_format);
	}
}

void ColorVariantConverter::emit_as_sequence(ryml::NodeRef &node, const Color &color, const YAMLStyle::View &style) const {
	node |= ryml::SEQ;

	style.apply_flow_style(node);

	YAMLStyle::FloatFormat r_format = style.has_child("r") ? style.get_child("r").get_float_format() : style.get_float_format();
	YAMLStyle::FloatFormat g_format = style.has_child("g") ? style.get_child("g").get_float_format() : style.get_float_format();
	YAMLStyle::FloatFormat b_format = style.has_child("b") ? style.get_child("b").get_float_format() : style.get_float_format();

	node.append_child() << float_to_string(color.r, r_format);
	node.append_child() << float_to_string(color.g, g_format);
	node.append_child() << float_to_string(color.b, b_format);

	if (color.a < 1.0f) {
		YAMLStyle::FloatFormat a_format = style.has_child("a") ? style.get_child("a").get_float_format() : style.get_float_format();
		node.append_child() << float_to_string(color.a, a_format);
	}
}

Variant ColorVariantConverter::decode(const ryml::ConstNodeRef &node, ParserContext *context) const {
	try {
		if (node.is_map()) {
			return decode_map(node, context);
		}

		if (node.is_seq()) {
			return decode_sequence(node, context);
		}

		if (node.has_val() && !node.val_is_null()) {
			return decode_string(node);
		}

		throw create_invalid_format_exception(node);
	} catch (const YAMLException &) {
		throw; // Re-throw YAML exceptions
	} catch (const std::exception &e) {
		throw create_decode_error_exception(e.what(), node);
	}
}

Variant ColorVariantConverter::decode_map(const ryml::ConstNodeRef &node, ParserContext *context) const {
	check_required_fields(node, { "r", "g", "b" });

	if (context->detect_style) {
		Ref<YAMLStyle> style = context->current_style();
		YAMLStyle::detect_flow_style(node, style);
		style->set_container_form(YAMLStyle::FORM_MAP);

		context->push_style("r");
	}

	YAMLStyle::FloatFormat r_format;
	real_t r = string_to_float<real_t>(node["r"].val(), &r_format);

	if (context->detect_style) {
		context->current_style()->set_float_format(r_format);
		context->pop_style();
		context->push_style("g");
	}

	YAMLStyle::FloatFormat g_format;
	real_t g = string_to_float<real_t>(node["g"].val(), &g_format);

	if (context->detect_style) {
		context->current_style()->set_float_format(g_format);
		context->pop_style();
		context->push_style("b");
	}

	YAMLStyle::FloatFormat b_format;
	real_t b = string_to_float<real_t>(node["b"].val(), &b_format);

	if (context->detect_style) {
		context->current_style()->set_float_format(b_format);
		context->pop_style();
		context->push_style("a");
	}

	YAMLStyle::FloatFormat a_format;
	real_t a = node.has_child("a") ? string_to_float<real_t>(node["a"].val(), &a_format) : 1.0f;

	if (context->detect_style) {
		context->current_style()->set_float_format(a_format);
		context->pop_style();
	}

	check_negative(r, g, b, a, node);

	return Color(r, g, b, a);
}

Variant ColorVariantConverter::decode_sequence(const ryml::ConstNodeRef &node, ParserContext *context) const {
	const size_t size = node.num_children();
	if (size != 3 && size != 4) {
		throw create_exception("Color sequence must have 3 or 4 elements (RGB[A])", node);
	}

	if (context->detect_style) {
		Ref<YAMLStyle> style = context->current_style();
		YAMLStyle::detect_flow_style(node, style);
		style->set_container_form(YAMLStyle::FORM_SEQ);

		context->push_style("r");
	}

	YAMLStyle::FloatFormat r_format;
	real_t r = string_to_float<real_t>(node[0].val(), &r_format);

	if (context->detect_style) {
		context->current_style()->set_float_format(r_format);
		context->pop_style();
		context->push_style("g");
	}

	YAMLStyle::FloatFormat g_format;
	real_t g = string_to_float<real_t>(node[1].val(), &g_format);

	if (context->detect_style) {
		context->current_style()->set_float_format(g_format);
		context->pop_style();
		context->push_style("b");
	}

	YAMLStyle::FloatFormat b_format;
	real_t b = string_to_float<real_t>(node[2].val(), &b_format);

	if (context->detect_style) {
		context->current_style()->set_float_format(b_format);
		context->pop_style();
		context->push_style("a");
	}

	YAMLStyle::FloatFormat a_format;
	real_t a = size == 4 ? string_to_float<real_t>(node[3].val(), &a_format) : 1.0f;

	if (context->detect_style) {
		context->current_style()->set_float_format(size == 4 ? a_format : r_format);
		context->pop_style();
	}

	check_negative(r, g, b, a, node);

	return Color(r, g, b, a);
}

Variant ColorVariantConverter::decode_string(const ryml::ConstNodeRef &node) const {
	String str = from_ryml_str(node.val());

	if (!Color::html_is_valid(str) && Color::find_named_color(str) == -1) {
		throw create_exception(vformat("Invalid Color string: %s", str), node);
	}

	return Color::from_string(str, Color(1.0f, 1.0f, 1.0f, 1.0f));
}

void ColorVariantConverter::check_negative(real_t r, real_t g, real_t b, real_t a, const ryml::ConstNodeRef &node) const {
	if (r < 0.0f) {
		throw create_exception("Negative Color component value (r)", node);
	}

	if (g < 0.0f) {
		throw create_exception("Negative Color component value (g)", node);
	}

	if (b < 0.0f) {
		throw create_exception("Negative Color component value (b)", node);
	}

	if (a < 0.0f) {
		throw create_exception("Negative Color component value (a)", node);
	}
}

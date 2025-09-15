#include "color_variant_converter.hpp"
#include "../exception.hpp"
#include "../util_numeric.hpp"

using namespace godot;

void YAMLColorVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const Color color = v.operator Color();

	if (style.get_container_form() != YAMLStyle::FORM_ANY) {
		ryml::NodeRef r_node;
		ryml::NodeRef g_node;
		ryml::NodeRef b_node;
		ryml::NodeRef a_node;

		if (style.get_container_form() == YAMLStyle::FORM_DICTIONARY) {
			node |= ryml::MAP;
			r_node = node["r"];
			g_node = node["g"];
			b_node = node["b"];
			if (color.a < 1.0f) {
				a_node = node["a"];
			}
		} else {
			node |= ryml::SEQ;
			r_node = node.append_child();
			g_node = node.append_child();
			b_node = node.append_child();
			if (color.a < 1.0f) {
				a_node = node.append_child();
			}
		}

		r_node << float_to_string(color.r, style.has_child("r") ? style.get_child("r").get_float_format() : style.get_float_format());
		g_node << float_to_string(color.g, style.has_child("g") ? style.get_child("g").get_float_format() : style.get_float_format());
		b_node << float_to_string(color.b, style.has_child("b") ? style.get_child("b").get_float_format() : style.get_float_format());
		if (color.a < 1.0f) {
			a_node << float_to_string(color.a, style.has_child("a") ? style.get_child("a").get_float_format() : style.get_float_format());
		}
	} else {
		node << store_string(color.to_html(color.a < 1.0f));
	}
}

Variant YAMLColorVariantConverter::decode(const ryml::ConstNodeRef &node, YAMLParserContext *context) const {
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

Variant YAMLColorVariantConverter::decode_map(const ryml::ConstNodeRef &node, YAMLParserContext *context) const {
	check_required_fields(node, { "r", "g", "b" });

	if (context->detect_style) {
		Ref<YAMLStyle> style = context->current_style();
		YAMLStyle::detect_flow_style(node, style);
		style->set_container_form(YAMLStyle::FORM_DICTIONARY);

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

Variant YAMLColorVariantConverter::decode_sequence(const ryml::ConstNodeRef &node, YAMLParserContext *context) const {
	const size_t size = node.num_children();
	if (size != 3 && size != 4) {
		throw create_exception("Color array must have 3 or 4 elements (RGB[A])", node);
	}

	if (context->detect_style) {
		Ref<YAMLStyle> style = context->current_style();
		YAMLStyle::detect_flow_style(node, style);
		style->set_container_form(YAMLStyle::FORM_ARRAY);

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

Variant YAMLColorVariantConverter::decode_string(const ryml::ConstNodeRef &node) const {
	String str = from_ryml_str(node.val());

	if (!Color::html_is_valid(str) && Color::find_named_color(str) == -1) {
		throw create_exception(vformat("Invalid Color string: %s", str), node);
	}

	return Color::from_string(str, Color(1.0f, 1.0f, 1.0f, 1.0f));
}

void YAMLColorVariantConverter::check_negative(real_t r, real_t g, real_t b, real_t a, const ryml::ConstNodeRef &node) const {
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

#include "packed_float64_array_yaml.h"
#include "../exception.h"
#include "../util_numeric.h"

using namespace godot;

void PackedFloat64ArrayVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const PackedFloat64Array array = v.operator PackedFloat64Array();
	node |= ryml::SEQ;

	if (array.size() == 0) {
		return; // Empty sequence
	}

	style.apply_flow_style(node);

	// Get template style if it exists (key "_template" is a convention for shared array item styling)
	YAMLStyle::View template_style = style.get_template_style();

	for (int i = 0; i < array.size(); ++i) {
		ryml::NodeRef value_node = node.append_child();

		YAMLStyle::View item_style;

		// Check for individual item style
		if (style.is_valid()) {
			const String idx = String::num_int64(i);
			if (style.has_child(idx)) {
				item_style = style.get_child(idx);
			}
		}

		// Fall back to shared style
		if (!item_style.is_valid()) {
			item_style = template_style;
		}

		value_node << float_to_string(array[i], item_style.get_float_format());
	}
}

Variant PackedFloat64ArrayVariantConverter::decode(const ryml::ConstNodeRef &node, ParserContext *context) const {
	if (!node.is_seq()) {
		throw create_invalid_format_exception(node);
	}

	const size_t size = node.num_children();
	PackedFloat64Array array;
	array.resize(size);

	const bool detect_style = context->detect_style;

	if (detect_style) {
		Ref<YAMLStyle> style = context->current_style();
		YAMLStyle::detect_flow_style(node, style);
		style->set_container_form(YAMLStyle::FORM_SEQ);
	}

	for (size_t i = 0; i < size; ++i) {
		try {
			if (detect_style) {
				context->push_style(String::num_uint64(i));
			}

			YAMLStyle::FloatFormat float_format;
			array.set(i, string_to_float<double>(node[i].val(), detect_style ? &float_format : nullptr));

			if (detect_style) {
				context->current_style()->set_float_format(float_format);
				context->pop_style();
			}
		} catch (const YAMLException &e) {
			throw YAMLException(vformat("Failed to decode PackedFloat64Array value at index %d: %s", i, e.what()), e.get_location());
		} catch (const std::exception &e) {
			throw YAMLException(vformat("Failed to decode PackedFloat64Array value at index %d: %s", i, e.what()), context->get_ryml_parser()->location(node[i]));
		}
	}

	return array;
}

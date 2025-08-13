#include "packed_float32_array_yaml.h"
#include "../exception.h"
#include "../util_numeric.h"

using namespace godot;

void PackedFloat32ArrayVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const PackedFloat32Array array = v.operator PackedFloat32Array();
	node |= ryml::SEQ;

	if (array.size() == 0) {
		return; // Empty array
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

Variant PackedFloat32ArrayVariantConverter::decode(const ryml::ConstNodeRef &node, ParserContext *context) const {
	if (!node.is_seq()) {
		throw create_invalid_format_exception(node);
	}

	const size_t size = node.num_children();
	PackedFloat32Array array;
	array.resize(size);

	const bool detect_style = context->detect_style;

	Ref<YAMLStyle> style;

	if (detect_style) {
		style = context->current_style();
		YAMLStyle::detect_flow_style(node, style);
		style->set_container_form(YAMLStyle::FORM_ARRAY);
	}

	for (size_t i = 0; i < size; ++i) {
		try {
			if (detect_style) {
				context->push_style(String::num_uint64(i));
			}

			YAMLStyle::FloatFormat float_format;
			array.set(i, string_to_float<float>(node[i].val(), detect_style ? &float_format : nullptr));

			if (detect_style) {
				context->current_style()->set_float_format(float_format);

				// First element style is used for template
				if (i == 0) {
					style->set_child("_template", context->current_style()->clone());
				}

				context->pop_style();
			}
		} catch (const YAMLException &e) {
			// NOTE: Cast size_t to int64_t for ARM64 macOS Variant compatibility
			throw YAMLException(vformat("Failed to decode PackedFloat32Array value at index %d: %s", static_cast<int64_t>(i), e.what()), e.get_location());
		} catch (const std::exception &e) {
			// NOTE: Cast size_t to int64_t for ARM64 macOS Variant compatibility
			throw YAMLException(vformat("Failed to decode PackedFloat32Array value at index %d: %s", static_cast<int64_t>(i), e.what()), context->get_ryml_parser()->location(node[i]));
		}
	}

	if (detect_style) {
		context->current_style()->simplify();
	}

	return array;
}

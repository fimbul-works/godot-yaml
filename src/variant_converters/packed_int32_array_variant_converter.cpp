#include "packed_int32_array_variant_converter.hpp"
#include "../exception.hpp"
#include "../util/util_numeric.hpp"

using namespace godot;

void YAMLPackedInt32ArrayVariantConverter::encode(
		ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const PackedInt32Array array = v.operator PackedInt32Array();
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

		value_node << int_to_string(array[i], item_style.get_integer_format());
	}
}

Variant YAMLPackedInt32ArrayVariantConverter::decode(const ryml::ConstNodeRef &node, YAMLParserContext *context) const {
	if (!node.is_seq()) {
		throw create_invalid_format_exception(node);
	}

	const size_t size = node.num_children();
	PackedInt32Array array;
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

			YAMLStyle::IntegerFormat int_format;
			int64_t value = string_to_int<int64_t>(node[i].val(), detect_style ? &int_format : nullptr);

			if (value < INT32_MIN || value > INT32_MAX) {
				// NOTE: Cast size_t to int64_t for ARM64 macOS Variant compatibility
				throw YAMLException(
						vformat("Failed to decode PackedInt32Array value at index %d: Integer value out of range",
								static_cast<int64_t>(i)),
						node[i].location(*context->get_ryml_parser()));
			}

			array.set(i, static_cast<int32_t>(value));

			if (detect_style) {
				context->current_style()->set_integer_format(int_format);

				// First element style is used for template
				if (i == 0) {
					style->set_child("_template", context->current_style()->clone());
				}

				context->pop_style();
			}
		} catch (const YAMLException &e) {
			// NOTE: Cast size_t to int64_t for ARM64 macOS Variant compatibility
			throw YAMLException(vformat("Failed to decode PackedInt32Array value at index %d: %s",
										static_cast<int64_t>(i), e.what()),
					e.get_location());
		} catch (const std::exception &e) {
			// NOTE: Cast size_t to int64_t for ARM64 macOS Variant compatibility
			throw YAMLException(vformat("Failed to decode PackedInt32Array value at index %d: %s",
										static_cast<int64_t>(i), e.what()),
					node[i].location(*context->get_ryml_parser()));
		}
	}

	if (detect_style) {
		context->current_style()->simplify();
	}

	return array;
}

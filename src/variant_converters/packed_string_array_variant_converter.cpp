#include "packed_string_array_variant_converter.hpp"
#include "../exception.hpp"

using namespace godot;

void YAMLPackedStringArrayVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const PackedStringArray array = v.operator PackedStringArray();
	node |= ryml::SEQ;

	if (array.size() == 0) {
		return; // Empty sequence
	}

	style.apply_flow_style(node);

	// Get template style if it exists (key "_template" is a convention for shared array item styling)
	YAMLStyle::View template_style = style.get_template_style();

	for (int i = 0; i < array.size(); ++i) {
		const String &str = array[i];
		ryml::NodeRef child = node.append_child();

		if (str.is_empty()) {
			child << ryml::csubstr{};
			continue;
		}

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

		if (item_style.is_valid()) {
			item_style.apply_string_style(child);
		} else {
			child |= ryml::VAL_DQUO;
		}

		child << to_ryml_str(str);
	}
}

Variant YAMLPackedStringArrayVariantConverter::decode(const ryml::ConstNodeRef &node, YAMLParserContext *context) const {
	if (!node.is_seq()) {
		throw create_invalid_format_exception(node);
	}

	const size_t size = node.num_children();
	PackedStringArray array;
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
			const ryml::ConstNodeRef &child = node[i];
			if (child.val_is_null() || !child.has_val()) {
				array.set(i, String());
			} else {
				array.set(i, from_ryml_str(child.val()));

				if (detect_style) {
					context->push_style(String::num_uint64(i));
					YAMLStyle::detect_string_style(child, context->current_style());

					// First element style is used for template
					if (i == 0) {
						style->set_child("_template", context->current_style()->clone());
					}

					context->pop_style();
				}
			}
		} catch (const YAMLException &e) {
			// NOTE: Cast size_t to int64_t for ARM64 macOS Variant compatibility
			throw YAMLException(vformat("Failed to decode PackedStringArray value at index %d: %s", static_cast<int64_t>(i), e.what()), e.get_location());
		} catch (const std::exception &e) {
			// NOTE: Cast size_t to int64_t for ARM64 macOS Variant compatibility
			throw YAMLException(vformat("Failed to decode PackedStringaArray value at index %d: %s", static_cast<int64_t>(i), e.what()), node[i].location(*context->get_ryml_parser()));
		}
	}

	if (detect_style) {
		context->current_style()->simplify();
	}

	return array;
}

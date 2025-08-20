#include "packed_color_array_yaml.h"
#include "../converter_factory.h"
#include "../exception.h"

using namespace godot;

PackedColorArrayVariantConverter::PackedColorArrayVariantConverter(ConverterFactory *factory) :
		color_converter(factory->create_converter_as<ColorVariantConverter>(Variant::COLOR)) {
	ERR_FAIL_NULL(color_converter);
}

void PackedColorArrayVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const PackedColorArray array = v.operator PackedColorArray();
	node |= ryml::SEQ;

	if (array.size() == 0) {
		return; // Empty array
	}

	style.apply_flow_style(node);

	// Get template style if it exists (key "_template" is a convention for shared array item styling)
	YAMLStyle::View template_style = style.get_template_style();

	for (int i = 0; i < array.size(); ++i) {
		ryml::NodeRef color_node = node.append_child();

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

		color_converter->encode(color_node, array[i], item_style);
	}
}

Variant PackedColorArrayVariantConverter::decode(const ryml::ConstNodeRef &node, ParserContext *context) const {
	if (!node.is_seq()) {
		throw create_invalid_format_exception(node);
	}

	PackedColorArray array;
	const size_t size = node.num_children();
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

			Color color = color_converter->decode(node[i], context);
			array.set(i, color);

			if (detect_style) {
				// First element style is used for template
				if (i == 0) {
					style->set_child("_template", context->current_style()->clone());
				}

				context->pop_style();
			}
		} catch (const YAMLException &e) {
			// NOTE: Cast size_t to int64_t for ARM64 macOS Variant compatibility
			throw YAMLException(vformat("Failed to decode PackedColorArray value at index %d: %s", static_cast<int64_t>(i), e.what()), e.get_location());
		} catch (const std::exception &e) {
			// NOTE: Cast size_t to int64_t for ARM64 macOS Variant compatibility
			throw YAMLException(vformat("Failed to decode PackedColorArray value at index %d: %s", static_cast<int64_t>(i), e.what()), context->get_ryml_parser()->location(node[i]));
		}
	}

	if (detect_style) {
		context->current_style()->simplify();
	}

	return array;
}

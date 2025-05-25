#include "packed_vector2_array_yaml.h"
#include "../converter_factory.h"
#include "../exception.h"

using namespace godot;

PackedVector2ArrayVariantConverter::PackedVector2ArrayVariantConverter(ConverterFactory *factory) :
		vec2_converter(factory->create_converter_as<Vector2VariantConverter>(Variant::VECTOR2)) {
	ERR_FAIL_NULL(vec2_converter);
}

void PackedVector2ArrayVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const PackedVector2Array array = v.operator PackedVector2Array();
	node |= ryml::SEQ;

	if (array.size() == 0) {
		return; // Empty array
	}

	style.apply_flow_style(node);

	// Get template style if it exists (key "_template" is a convention for shared array item styling)
	YAMLStyle::View template_style = style.get_template_style();

	for (int i = 0; i < array.size(); ++i) {
		ryml::NodeRef vec_node = node.append_child();

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

		vec2_converter->encode(vec_node, array[i], item_style);
	}
}

Variant PackedVector2ArrayVariantConverter::decode(const ryml::ConstNodeRef &node, ParserContext *context) const {
	if (!node.is_seq()) {
		throw create_invalid_format_exception(node);
	}

	PackedVector2Array array;
	const size_t size = node.num_children();
	array.resize(size);

	const bool detect_style = context->detect_style;

	Ref<YAMLStyle> style;

	if (detect_style) {
		style = context->current_style();
		YAMLStyle::detect_flow_style(node, style);
		style->set_container_form(YAMLStyle::FORM_ARRAY);
	}

	if (size > 0) {
		for (size_t i = 0; i < size; ++i) {
			try {
				if (detect_style) {
					context->push_style(String::num_uint64(i));
				}

				Vector2 vec2 = vec2_converter->decode(node[i], context);
				array.set(i, vec2);

				if (detect_style) {
					// First element style is used for template
					if (i == 0) {
						style->set_child("_template", context->current_style()->clone());
					}

					context->pop_style();
				}
			} catch (const YAMLException &e) {
				throw YAMLException(vformat("Failed to decode PackedVector2Array value at index %d: %s", i, e.what()), e.get_location());
			} catch (const std::exception &e) {
				throw YAMLException(vformat("Failed to decode PackedVector2Array value at index %d: %s", i, e.what()), context->get_ryml_parser()->location(node[i]));
			}
		}
	}

	if (detect_style) {
		context->current_style()->simplify();
	}

	return array;
}

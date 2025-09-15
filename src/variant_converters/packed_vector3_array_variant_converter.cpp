#include "packed_vector3_array_variant_converter.hpp"
#include "../exception.hpp"
#include "variant_converter_factory.hpp"

using namespace godot;

YAMLPackedVector3ArrayVariantConverter::YAMLPackedVector3ArrayVariantConverter(YAMLVariantConverterFactory *factory) :
		vec3_converter(factory->create_converter_as<YAMLVector3VariantConverter>(Variant::VECTOR3)) {
	ERR_FAIL_NULL(vec3_converter);
}

void YAMLPackedVector3ArrayVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const PackedVector3Array array = v.operator PackedVector3Array();
	node |= ryml::SEQ;

	if (array.size() == 0) {
		return; // Empty sequence
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

		vec3_converter->encode(vec_node, array[i], item_style);
	}
}

Variant YAMLPackedVector3ArrayVariantConverter::decode(const ryml::ConstNodeRef &node, YAMLParserContext *context) const {
	if (!node.is_seq()) {
		throw create_invalid_format_exception(node);
	}

	PackedVector3Array array;
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

				Vector3 vec3 = vec3_converter->decode(node[i], context);
				array.set(i, vec3);

				if (detect_style) {
					// First element style is used for template
					if (i == 0) {
						style->set_child("_template", context->current_style()->clone());
					}

					context->pop_style();
				}
			} catch (const YAMLException &e) {
				// NOTE: Cast size_t to int64_t for ARM64 macOS Variant compatibility
				throw YAMLException(vformat("Failed to decode PackedVector3Array value at index %d: %s", static_cast<int64_t>(i), e.what()), e.get_location());
			} catch (const std::exception &e) {
				// NOTE: Cast size_t to int64_t for ARM64 macOS Variant compatibility
				throw YAMLException(vformat("Failed to decode PackedVector3Array value at index %d: %s", static_cast<int64_t>(i), e.what()), context->get_ryml_parser()->location(node[i]));
			}
		}
	}

	if (detect_style) {
		context->current_style()->simplify();
	}

	return array;
}

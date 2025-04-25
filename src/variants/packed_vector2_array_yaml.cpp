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
		return; // Empty sequence
	}

	style.apply_flow_style(node);

	// Get shared item style if it exists (key "_template" is a convention for shared array item styling)
	YAMLStyle::View shared_item_style;
	if (style.is_valid()) {
		shared_item_style = style.get_child("_template");
	}

	for (int i = 0; i < array.size(); ++i) {
		ryml::NodeRef vec_node = node.append_child();

		YAMLStyle::View item_style;
		if (style.is_valid()) {
			// Check for individual item style
			const String idx = String::num_int64(i);
			if (style.has_child(idx)) {
				item_style = style.get_child(idx);
			}

			// Fall back to shared style
			if (!item_style.is_valid()) {
				item_style = shared_item_style;
			}
		}

		vec2_converter->encode(vec_node, array[i], item_style);
	}
}

Variant PackedVector2ArrayVariantConverter::decode(const ryml::ConstNodeRef &node) const {
	if (!node.is_seq()) {
		throw create_invalid_format_exception("PackedVector2Array", node);
	}

	PackedVector2Array array;
	const size_t size = node.num_children();
	array.resize(size);

	if (size > 0) {
		for (size_t i = 0; i < size; ++i) {
			try {
				Vector2 vec2 = vec2_converter->decode(node[i]);
				array.set(i, vec2);
			} catch (const std::exception &e) {
				throw create_decode_error_exception(vformat("PackedVector2Array value at index %d", i).utf8().get_data(), e.what(), node);
			}
		}
	}

	return array;
}

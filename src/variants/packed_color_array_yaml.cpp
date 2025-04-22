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
		return; // Empty sequence
	}

	// Apply flow style to the sequence itself if specified
	style.apply_flow_style(node);

	// Get shared item style if it exists (key "_items" is a convention for shared array item styling)
	YAMLStyle::View shared_item_style;
	if (style.is_valid()) {
		shared_item_style = style.get_child("_items");
	}

	for (int i = 0; i < array.size(); ++i) {
		ryml::NodeRef color_node = node.append_child();

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

		color_converter->encode(color_node, array[i], item_style);
	}
}

Variant PackedColorArrayVariantConverter::decode(const ryml::ConstNodeRef &node) const {
	if (!node.is_seq()) {
		throw YAMLException::create_invalid_format("PackedColorArray");
	}

	PackedColorArray array;
	const size_t size = node.num_children();
	array.resize(size);

	if (size > 0) {
		for (size_t i = 0; i < size; ++i) {
			try {
				Color color = color_converter->decode(node[i]);
				array.set(i, color);
			} catch (const std::exception &e) {
				throw YAMLException::create_decode_error(String("PackedColorArray value at index " + String::num_uint64(i)).utf8().get_data(), e.what());
			}
		}
	}

	return array;
}

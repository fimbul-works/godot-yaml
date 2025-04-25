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

	// Get shared item style if it exists (key "_template" is a convention for shared array item styling)
	YAMLStyle::View shared_item_style;
	if (style.is_valid()) {
		shared_item_style = style.get_child("_template");
	}

	for (int i = 0; i < array.size(); ++i) {
		ryml::NodeRef value_node = node.append_child();

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

		value_node << float_to_string(array[i], item_style.get_float_format());
	}
}

Variant PackedFloat64ArrayVariantConverter::decode(const ryml::ConstNodeRef &node) const {
	if (!node.is_seq()) {
		throw create_invalid_format_exception("PackedFloat64Array", node);
	}

	const size_t size = node.num_children();
	PackedFloat64Array array;
	array.resize(size);

	for (size_t i = 0; i < size; ++i) {
		try {
			array.set(i, string_to_float<double>(node[i].val()));
		} catch (const std::exception &e) {
			throw create_decode_error_exception(vformat("PackedFloat64Array value at index %d", i).utf8().get_data(), e.what(), node);
		}
	}

	return array;
}

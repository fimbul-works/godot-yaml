#include "packed_string_array_yaml.h"
#include "../exception.h"

using namespace godot;

void PackedStringArrayVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const PackedStringArray array = v.operator PackedStringArray();
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
		const String &str = array[i];
		ryml::NodeRef child = node.append_child();

		if (str.is_empty()) {
			child << ryml::csubstr{};
			continue;
		}

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

		if (item_style.is_valid()) {
			item_style.apply_string_style(child);
		} else {
			child |= ryml::VAL_DQUO;
		}

		child << to_ryml_str(str);
	}
}

Variant PackedStringArrayVariantConverter::decode(const ryml::ConstNodeRef &node) const {
	if (!node.is_seq()) {
		throw YAMLException::create_invalid_format("PackedStringArray");
	}

	const size_t size = node.num_children();
	PackedStringArray array;
	array.resize(size);

	for (size_t i = 0; i < size; ++i) {
		try {
			const ryml::ConstNodeRef &child = node[i];
			if (child.val_is_null() || !child.has_val()) {
				array.set(i, String());
			} else {
				array.set(i, from_ryml_str(child.val()));
			}
		} catch (const std::exception &e) {
			throw YAMLException::create_decode_error(String("PackedStringArray value at index " + String::num_uint64(i)).utf8().get_data(), e.what());
		}
	}

	return array;
}

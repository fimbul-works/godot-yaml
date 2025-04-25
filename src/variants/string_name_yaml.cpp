#include "string_name_yaml.h"
#include "../exception.h"

using namespace godot;

void StringNameVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const StringName str = v.operator StringName();

	String string_val = String(str);
	if (string_val.is_empty()) {
		node << ryml::csubstr{};
	} else {
		if (style.is_valid()) {
			style.apply_string_style(node);
		} else {
			node |= ryml::VAL_DQUO;
		}
		node << to_ryml_str(string_val);
	}
}

Variant StringNameVariantConverter::decode(const ryml::ConstNodeRef &node) const {
	try {
		if (node.val_is_null()) {
			return StringName(); // Return empty StringName
		}

		if (!node.has_val()) {
			throw YAMLException::create_invalid_format("StringName");
		}

		return decode_from_string(node.val());
	} catch (const YAMLException &) {
		throw; // Re-throw YAML exceptions
	} catch (const std::exception &e) {
		throw YAMLException::create_decode_error("StringName", e.what());
	}
}

Variant StringNameVariantConverter::decode_from_string(const ryml::csubstr &val) const {
	String string_val = String::utf8(val.str, val.len);
	return StringName(string_val);
}

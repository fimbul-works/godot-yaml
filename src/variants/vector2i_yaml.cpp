#include "vector2i_yaml.h"
#include "../exception.h"
#include "../util_numeric.h"

using namespace godot;

void Vector2iVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const Vector2i vec = v.operator Vector2i();

	YAMLStyle::IntegerFormat int_format = style.get_integer_format();
	style.apply_flow_style(node);

	if (!style.is_valid() || style.get_container_form() != YAMLStyle::FORM_SEQ) {
		node |= ryml::MAP;

		node["x"] << int_to_string(vec.x, int_format);
		node["y"] << int_to_string(vec.y, int_format);
	} else {
		node |= ryml::SEQ;

		node.append_child() << int_to_string(vec.x, int_format);
		node.append_child() << int_to_string(vec.y, int_format);
	}
}

Variant Vector2iVariantConverter::decode(const ryml::ConstNodeRef &node) const {
	try {
		if (node.is_map()) {
			return decode_from_map(node);
		}

		if (node.is_seq()) {
			return decode_from_sequence(node);
		}

		throw YAMLException::create_invalid_format("Vector2i");
	} catch (const YAMLException &) {
		throw; // Re-throw YAML exceptions
	} catch (const std::exception &e) {
		throw YAMLException::create_decode_error("Vector2i", e.what());
	}
}

Variant Vector2iVariantConverter::decode_from_map(const ryml::ConstNodeRef &node) const {
	check_required_fields(node, { "x", "y" });

	return Vector2i(
			string_to_int<int32_t>(node["x"].val()),
			string_to_int<int32_t>(node["y"].val()));
}

Variant Vector2iVariantConverter::decode_from_sequence(const ryml::ConstNodeRef &node) const {
	if (node.num_children() != 2) {
		throw YAMLException::create_invalid_sequence_length("Vector2i", 2);
	}

	return Vector2i(
			string_to_int<int32_t>(node[0].val()),
			string_to_int<int32_t>(node[1].val()));
}

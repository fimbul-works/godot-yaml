#include "vector3i_yaml.h"
#include "../exception.h"
#include "../util_numeric.h"

using namespace godot;

void Vector3iVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const Vector3i vec = v.operator Vector3i();

	YAMLStyle::IntegerFormat int_format = style.get_integer_format();
	style.apply_flow_style(node);

	if (!style.is_valid() || style.get_container_form() != YAMLStyle::FORM_SEQ) {
		node |= ryml::MAP;

		node["x"] << int_to_string(vec.x, int_format);
		node["y"] << int_to_string(vec.y, int_format);
		node["z"] << int_to_string(vec.z, int_format);
	} else {
		node |= ryml::SEQ;

		node.append_child() << int_to_string(vec.x, int_format);
		node.append_child() << int_to_string(vec.y, int_format);
		node.append_child() << int_to_string(vec.z, int_format);
	}
}

Variant Vector3iVariantConverter::decode(const ryml::ConstNodeRef &node) const {
	try {
		if (node.is_map()) {
			return decode_from_map(node);
		}

		if (node.is_seq()) {
			return decode_from_sequence(node);
		}

		throw create_invalid_format_exception("Vector3i", node);
	} catch (const YAMLException &) {
		throw; // Re-throw YAML exceptions
	} catch (const std::exception &e) {
		throw create_decode_error_exception("Vector3i", e.what(), node);
	}
}

Variant Vector3iVariantConverter::decode_from_map(const ryml::ConstNodeRef &node) const {
	check_required_fields(node, { "x", "y", "z" });

	return Vector3i(
			string_to_int<int32_t>(node["x"].val()),
			string_to_int<int32_t>(node["y"].val()),
			string_to_int<int32_t>(node["z"].val()));
}

Variant Vector3iVariantConverter::decode_from_sequence(const ryml::ConstNodeRef &node) const {
	if (node.num_children() != 3) {
		throw create_invalid_sequence_length_exception("Vector3i", 3, node);
	}

	return Vector3i(
			string_to_int<int32_t>(node[0].val()),
			string_to_int<int32_t>(node[1].val()),
			string_to_int<int32_t>(node[2].val()));
}

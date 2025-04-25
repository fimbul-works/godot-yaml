#include "vector4i_yaml.h"
#include "../exception.h"
#include "../util_numeric.h"

using namespace godot;

void Vector4iVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const Vector4i vec = v.operator Vector4i();

	YAMLStyle::IntegerFormat int_format = style.get_integer_format();
	style.apply_flow_style(node);

	if (!style.is_valid() || style.get_container_form() != YAMLStyle::FORM_SEQ) {
		node |= ryml::MAP;

		node["x"] << int_to_string(vec.x, int_format);
		node["y"] << int_to_string(vec.y, int_format);
		node["z"] << int_to_string(vec.z, int_format);
		node["w"] << int_to_string(vec.w, int_format);
	} else {
		node |= ryml::SEQ;

		node.append_child() << int_to_string(vec.x, int_format);
		node.append_child() << int_to_string(vec.y, int_format);
		node.append_child() << int_to_string(vec.z, int_format);
		node.append_child() << int_to_string(vec.w, int_format);
	}
}

Variant Vector4iVariantConverter::decode(const ryml::ConstNodeRef &node) const {
	try {
		if (node.is_map()) {
			return decode_from_map(node);
		}

		if (node.is_seq()) {
			return decode_from_sequence(node);
		}

		throw YAMLException::create_invalid_format("Vector4i");
	} catch (const YAMLException &) {
		throw; // Re-throw YAML exceptions
	} catch (const std::exception &e) {
		throw YAMLException::create_decode_error("Vector4i", e.what());
	}
}

Variant Vector4iVariantConverter::decode_from_map(const ryml::ConstNodeRef &node) const {
	check_required_fields(node, { "x", "y", "z", "w" });

	return Vector4i(
			string_to_int<int32_t>(node["x"].val()),
			string_to_int<int32_t>(node["y"].val()),
			string_to_int<int32_t>(node["z"].val()),
			string_to_int<int32_t>(node["w"].val()));
}

Variant Vector4iVariantConverter::decode_from_sequence(const ryml::ConstNodeRef &node) const {
	if (node.num_children() != 4) {
		throw YAMLException::create_invalid_sequence_length("Vector4i", 4);
	}

	return Vector4i(
			string_to_int<int32_t>(node[0].val()),
			string_to_int<int32_t>(node[1].val()),
			string_to_int<int32_t>(node[2].val()),
			string_to_int<int32_t>(node[3].val()));
}

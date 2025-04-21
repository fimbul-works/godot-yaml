#include "vector3i_yaml.h"
#include "../exception.h"
#include "../util_numeric.h"

using namespace godot;

void Vector3iVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const Vector3i vec = v.operator Vector3i();

	if (!style.is_valid() || style.get_container_form() != YAMLStyle::FORM_SEQ) {
		// Map styles
		node |= ryml::MAP;

		// Flow style
		if (!style.is_valid()) {
			node |= ryml::FLOW_SL;
		} else {
			style.apply_flow_style(node);
		}

		node["x"] << int_to_string(vec.x);
		node["y"] << int_to_string(vec.y);
		node["z"] << int_to_string(vec.z);
	} else {
		// Collection styles
		node |= ryml::SEQ;

		// Flow style
		if (!style.is_valid()) {
			node |= ryml::FLOW_SL;
		} else {
			style.apply_flow_style(node);
		}

		node.append_child() << int_to_string(vec.x);
		node.append_child() << int_to_string(vec.y);
		node.append_child() << int_to_string(vec.z);
	}
}

Variant Vector3iVariantConverter::decode(const ryml::ConstNodeRef &node) const {
	try {
		if (node.is_map()) {
			return decode_from_map(node);
		} else if (node.is_seq()) {
			return decode_from_sequence(node);
		}
		throw YAMLException::create_invalid_format("Vector3i");
	} catch (const YAMLException &) {
		throw;
	} catch (const std::exception &e) {
		throw YAMLException::create_decode_error("Vector3i", e.what());
	}
}

Variant Vector3iVariantConverter::decode_from_map(const ryml::ConstNodeRef &node) const {
	if (!node.has_child("x") || !node.has_child("y") || !node.has_child("z")) {
		throw YAMLException::create_missing_field("Vector3i", "x, y, z");
	}

	return Vector3i(
			string_to_int<int32_t>(node["x"].val()),
			string_to_int<int32_t>(node["y"].val()),
			string_to_int<int32_t>(node["z"].val()));
}

Variant Vector3iVariantConverter::decode_from_sequence(const ryml::ConstNodeRef &node) const {
	if (node.num_children() != 3) {
		throw YAMLException::create_invalid_sequence_length("Vector3i", 3);
	}

	return Vector3i(
			string_to_int<int32_t>(node[0].val()),
			string_to_int<int32_t>(node[1].val()),
			string_to_int<int32_t>(node[2].val()));
}

#include "vector4i_yaml.h"
#include "../exception.h"
#include "../util_numeric.h"

using namespace godot;

void Vector4iVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const Vector4i vec = v.operator Vector4i();

	YAMLStyle::ContainerForm container_form = style.get_container_form();
	if (!style.is_valid() || container_form != YAMLStyle::FORM_SEQ) {
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
		node["w"] << int_to_string(vec.w);
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
		node.append_child() << int_to_string(vec.w);
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

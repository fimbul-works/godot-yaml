#include "vector4_yaml.h"
#include "../exception.h"
#include "../util_numeric.h"

using namespace godot;

void Vector4VariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const Vector4 vec = v.operator Vector4();

	if (!style.is_valid() || style.get_container_form() != YAMLStyle::FORM_SEQ) {
		// Map styles
		node |= ryml::MAP;

		// Flow style
		if (!style.is_valid()) {
			node |= ryml::FLOW_SL;
		} else {
			style.apply_flow_style(node);
		}

		node["x"] << float_to_string(vec.x);
		node["y"] << float_to_string(vec.y);
		node["z"] << float_to_string(vec.z);
		node["w"] << float_to_string(vec.w);
	} else {
		// Collection styles
		node |= ryml::SEQ;

		// Flow style
		if (!style.is_valid()) {
			node |= ryml::FLOW_SL;
		} else {
			style.apply_flow_style(node);
		}

		node.append_child() << float_to_string(vec.x);
		node.append_child() << float_to_string(vec.y);
		node.append_child() << float_to_string(vec.z);
		node.append_child() << float_to_string(vec.w);
	}
}

Variant Vector4VariantConverter::decode(const ryml::ConstNodeRef &node) const {
	try {
		if (node.is_map()) {
			return decode_from_map(node);
		}

		if (node.is_seq()) {
			return decode_from_sequence(node);
		}

		throw YAMLException::create_invalid_format("Vector4");
	} catch (const YAMLException &) {
		throw; // Re-throw YAML exceptions
	} catch (const std::exception &e) {
		throw YAMLException::create_decode_error("Vector4", e.what());
	}
}

Variant Vector4VariantConverter::decode_from_map(const ryml::ConstNodeRef &node) const {
	check_required_fields(node, { "x", "y", "z", "w" });

	return Vector4(
			string_to_float<real_t>(node["x"].val()),
			string_to_float<real_t>(node["y"].val()),
			string_to_float<real_t>(node["z"].val()),
			string_to_float<real_t>(node["w"].val()));
}

Variant Vector4VariantConverter::decode_from_sequence(const ryml::ConstNodeRef &node) const {
	if (node.num_children() != 4) {
		throw YAMLException::create_invalid_sequence_length("Vector4", 4);
	}

	return Vector4(
			string_to_float<real_t>(node[0].val()),
			string_to_float<real_t>(node[1].val()),
			string_to_float<real_t>(node[2].val()),
			string_to_float<real_t>(node[3].val()));
}

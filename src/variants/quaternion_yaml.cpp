#include "quaternion_yaml.h"
#include "../exception.h"
#include "../util_numeric.h"

using namespace godot;

void QuaternionVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const Quaternion quat = v.operator Quaternion();
	if (!style.is_valid() || style.get_container_form() != YAMLStyle::FORM_SEQ) {
		// Map styles
		node |= ryml::MAP;

		// Flow style
		if (!style.is_valid()) {
			node |= ryml::FLOW_SL;
		} else {
			style.apply_flow_style(node);
		}

		node["x"] << float_to_string(quat.x);
		node["y"] << float_to_string(quat.y);
		node["z"] << float_to_string(quat.z);
		node["w"] << float_to_string(quat.w);
	} else {
		// Collection styles
		node |= ryml::SEQ;

		// Flow style
		if (!style.is_valid()) {
			node |= ryml::FLOW_SL;
		} else {
			style.apply_flow_style(node);
		}

		node.append_child() << float_to_string(quat.x);
		node.append_child() << float_to_string(quat.y);
		node.append_child() << float_to_string(quat.z);
		node.append_child() << float_to_string(quat.w);
	}
}

Variant QuaternionVariantConverter::decode(const ryml::ConstNodeRef &node) const {
	try {
		if (node.is_map()) {
			return decode_from_map(node);
		} else if (node.is_seq()) {
			return decode_from_sequence(node);
		}
		throw YAMLException::create_invalid_format("Quaternion");
	} catch (const YAMLException &) {
		throw;
	} catch (const std::exception &e) {
		throw YAMLException::create_decode_error("Quaternion", e.what());
	}
}

Variant QuaternionVariantConverter::decode_from_map(const ryml::ConstNodeRef &node) const {
	check_required_fields(node, { "x", "y", "z", "w" });

	return Quaternion(
			string_to_float<real_t>(node["x"].val()),
			string_to_float<real_t>(node["y"].val()),
			string_to_float<real_t>(node["z"].val()),
			string_to_float<real_t>(node["w"].val()));
}

Variant QuaternionVariantConverter::decode_from_sequence(const ryml::ConstNodeRef &node) const {
	if (node.num_children() != 4) {
		throw YAMLException::create_invalid_sequence_length("Quaternion", 4);
	}

	return Quaternion(
			string_to_float<real_t>(node[0].val()),
			string_to_float<real_t>(node[1].val()),
			string_to_float<real_t>(node[2].val()),
			string_to_float<real_t>(node[3].val()));
}

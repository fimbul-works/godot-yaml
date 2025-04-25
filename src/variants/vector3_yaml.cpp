#include "vector3_yaml.h"
#include "../exception.h"
#include "../util_numeric.h"

using namespace godot;

void Vector3VariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const Vector3 vec = v.operator Vector3();

	YAMLStyle::FloatFormat float_format = style.get_float_format();
	style.apply_flow_style(node);

	if (!style.is_valid() || style.get_container_form() != YAMLStyle::FORM_SEQ) {
		node |= ryml::MAP;

		node["x"] << float_to_string(vec.x, float_format);
		node["y"] << float_to_string(vec.y, float_format);
		node["z"] << float_to_string(vec.z, float_format);
	} else {
		node |= ryml::SEQ;

		node.append_child() << float_to_string(vec.x, float_format);
		node.append_child() << float_to_string(vec.y, float_format);
		node.append_child() << float_to_string(vec.z, float_format);
	}
}

Variant Vector3VariantConverter::decode(const ryml::ConstNodeRef &node) const {
	try {
		if (node.is_map()) {
			return decode_from_map(node);
		}

		if (node.is_seq()) {
			return decode_from_sequence(node);
		}

		throw create_invalid_format_exception("Vector3", node);
	} catch (const YAMLException &) {
		throw; // Re-throw YAML exceptions
	} catch (const std::exception &e) {
		throw create_decode_error_exception("Vector3", e.what(), node);
	}
}

Variant Vector3VariantConverter::decode_from_map(const ryml::ConstNodeRef &node) const {
	check_required_fields(node, { "x", "y", "z" });

	return Vector3(
			string_to_float<real_t>(node["x"].val()),
			string_to_float<real_t>(node["y"].val()),
			string_to_float<real_t>(node["z"].val()));
}

Variant Vector3VariantConverter::decode_from_sequence(const ryml::ConstNodeRef &node) const {
	if (node.num_children() != 3) {
		throw create_invalid_sequence_length_exception("Vector3", 3, node);
	}

	return Vector3(
			string_to_float<real_t>(node[0].val()),
			string_to_float<real_t>(node[1].val()),
			string_to_float<real_t>(node[2].val()));
}

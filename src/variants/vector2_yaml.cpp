#include "vector2_yaml.h"
#include "../exception.h"
#include "../util_numeric.h"

using namespace godot;

void Vector2VariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const Vector2 vec = v.operator Vector2();

	YAMLStyle::FloatFormat float_format = style.get_float_format();
	style.apply_flow_style(node);

	if (!style.is_valid() || style.get_container_form() != YAMLStyle::FORM_SEQ) {
		node |= ryml::MAP;

		node["x"] << float_to_string(vec.x, float_format);
		node["y"] << float_to_string(vec.y, float_format);
	} else {
		node |= ryml::SEQ;

		node.append_child() << float_to_string(vec.x, float_format);
		node.append_child() << float_to_string(vec.y, float_format);
	}
}

Variant Vector2VariantConverter::decode(const ryml::ConstNodeRef &node) const {
	try {
		if (node.is_map()) {
			return decode_from_map(node);
		}

		if (node.is_seq()) {
			return decode_from_sequence(node);
		}

		throw YAMLException::create_invalid_format("Vector2");
	} catch (const YAMLException &) {
		throw; // Re-throw YAML exceptions
	} catch (const std::exception &e) {
		throw YAMLException::create_decode_error("Vector2", e.what());
	}
}

Variant Vector2VariantConverter::decode_from_map(const ryml::ConstNodeRef &node) const {
	check_required_fields(node, { "x", "y" });

	return Vector2(
			string_to_float<real_t>(node["x"].val()),
			string_to_float<real_t>(node["y"].val()));
}

Variant Vector2VariantConverter::decode_from_sequence(const ryml::ConstNodeRef &node) const {
	if (node.num_children() != 2) {
		throw YAMLException::create_invalid_sequence_length("Vector2", 2);
	}

	return Vector2(
			string_to_float<real_t>(node[0].val()),
			string_to_float<real_t>(node[1].val()));
}

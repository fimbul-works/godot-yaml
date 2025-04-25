#include "plane_yaml.h"
#include "../converter_factory.h"
#include "../exception.h"
#include "../util_numeric.h"

using namespace godot;

PlaneVariantConverter::PlaneVariantConverter(ConverterFactory *factory) :
		vec3_converter(factory->create_converter_as<Vector3VariantConverter>(Variant::VECTOR3)) {
	ERR_FAIL_NULL(vec3_converter);
}

void PlaneVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const Plane plane = v.operator Plane();

	if (!style.is_valid() || style.get_container_form() != YAMLStyle::FORM_SEQ) {
		emit_as_map(node, plane, style);
	} else {
		emit_as_sequence(node, plane, style);
	}
}

void PlaneVariantConverter::emit_as_map(ryml::NodeRef &node, const Plane &plane, const YAMLStyle::View &style) const {
	node |= ryml::MAP;

	style.apply_flow_style(node);

	YAMLStyle::View normal_style = style.is_valid() ? style.get_child("normal") : YAMLStyle::View();
	ryml::NodeRef normal_node = node["normal"];
	vec3_converter->encode(normal_node, plane.normal, normal_style);

	YAMLStyle::View d_style = style.is_valid() ? style.get_child("d") : YAMLStyle::View();
	node["d"] << float_to_string(plane.d, d_style.get_float_format());
}

void PlaneVariantConverter::emit_as_sequence(ryml::NodeRef &node, const Plane &plane, const YAMLStyle::View &style) const {
	node |= ryml::SEQ;

	style.apply_flow_style(node);

	YAMLStyle::View normal_style = style.is_valid() ? style.get_child("0") : YAMLStyle::View();
	ryml::NodeRef normal_node = node.append_child();
	vec3_converter->encode(normal_node, plane.normal, normal_style);

	YAMLStyle::View d_style = style.is_valid() ? style.get_child("1") : YAMLStyle::View();
	node.append_child() << float_to_string(plane.d, d_style.get_float_format());
}

Variant PlaneVariantConverter::decode(const ryml::ConstNodeRef &node) const {
	try {
		if (node.is_map()) {
			return decode_from_map(node);
		}

		if (node.is_seq()) {
			return decode_from_sequence(node);
		}

		throw YAMLException::create_invalid_format("Plane");
	} catch (const YAMLException &) {
		throw; // Re-throw YAML exceptions
	} catch (const std::exception &e) {
		throw YAMLException::create_decode_error("Plane", e.what());
	}
}

Variant PlaneVariantConverter::decode_from_map(const ryml::ConstNodeRef &node) const {
	check_required_fields(node, { "normal", "d" });

	Vector3 normal = vec3_converter->decode(node["normal"]).operator Vector3();
	real_t d = string_to_float<real_t>(node["d"].val());

	return Plane(normal, d);
}

Variant PlaneVariantConverter::decode_from_sequence(const ryml::ConstNodeRef &node) const {
	if (node.num_children() != 2) {
		throw YAMLException::create_invalid_sequence_length("Plane", 2);
	}

	Vector3 normal = vec3_converter->decode(node[0]).operator Vector3();
	real_t d = string_to_float<real_t>(node[1].val());

	return Plane(normal, d);
}

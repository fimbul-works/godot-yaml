#include "aabb_yaml.h"
#include "../converter_factory.h"
#include "../exception.h"

#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

AABBVariantConverter::AABBVariantConverter(ConverterFactory *factory) :
		vec3_converter(factory->create_converter_as<Vector3VariantConverter>(Variant::VECTOR3)) {
	ERR_FAIL_NULL(vec3_converter);
}

void AABBVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const AABB aabb = AABB(v);

	if (!style.is_valid() || style.get_container_form() != YAMLStyle::FORM_SEQ) {
		emit_as_map(node, aabb, style);
	} else {
		emit_as_sequence(node, aabb, style);
	}
}

void AABBVariantConverter::emit_as_map(ryml::NodeRef &node, const AABB &aabb, const YAMLStyle::View &style) const {
	node |= ryml::MAP;

	style.apply_flow_style(node);

	YAMLStyle::View position_style = style.is_valid() ? style.get_child("position") : YAMLStyle::View();
	YAMLStyle::View size_style = style.is_valid() ? style.get_child("size") : YAMLStyle::View();

	ryml::NodeRef position_node = node["position"];
	vec3_converter->encode(position_node, aabb.position, position_style);

	ryml::NodeRef size_node = node["size"];
	vec3_converter->encode(size_node, aabb.size, size_style);
}

void AABBVariantConverter::emit_as_sequence(ryml::NodeRef &node, const AABB &aabb, const YAMLStyle::View &style) const {
	node |= ryml::SEQ;

	style.apply_flow_style(node);

	YAMLStyle::View position_style = style.is_valid() ? style.get_child("0") : YAMLStyle::View();
	YAMLStyle::View size_style = style.is_valid() ? style.get_child("1") : YAMLStyle::View();

	ryml::NodeRef position_node = node.append_child();
	vec3_converter->encode(position_node, aabb.position, position_style);

	ryml::NodeRef size_node = node.append_child();
	vec3_converter->encode(size_node, aabb.size, size_style);
}

Variant AABBVariantConverter::decode(const ryml::ConstNodeRef &node) const {
	try {
		if (node.is_map()) {
			return decode_from_map(node);
		}

		if (node.is_seq()) {
			return decode_from_sequence(node);
		}

		throw create_invalid_format_exception("AABB", node);
	} catch (const YAMLException &) {
		throw; // Re-throw YAML exceptions
	} catch (const std::exception &e) {
		throw create_decode_error_exception("AABB", e.what(), node);
	}
}

Variant AABBVariantConverter::decode_from_map(const ryml::ConstNodeRef &node) const {
	check_required_fields(node, { "position", "size" });

	Vector3 position = vec3_converter->decode(node["position"]).operator Vector3();
	Vector3 size = vec3_converter->decode(node["size"]).operator Vector3();

	return AABB(position, size);
}

Variant AABBVariantConverter::decode_from_sequence(const ryml::ConstNodeRef &node) const {
	if (node.num_children() != 2) {
		throw create_invalid_sequence_length_exception("AABB", 2, node);
	}

	Vector3 position = vec3_converter->decode(node[0]).operator Vector3();
	Vector3 size = vec3_converter->decode(node[1]).operator Vector3();

	return AABB(position, size);
}

} // namespace godot

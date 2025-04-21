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

	// Flow style
	style.apply_flow_style(node);

	// Pass child styles down to nested converters
	YAMLStyle::View position_style = style.is_valid() ? style.get_child("position") : YAMLStyle::View();
	YAMLStyle::View size_style = style.is_valid() ? style.get_child("size") : YAMLStyle::View();

	ryml::NodeRef position_node = node["position"];
	vec3_converter->encode(position_node, aabb.position, position_style);

	ryml::NodeRef size_node = node["size"];
	vec3_converter->encode(size_node, aabb.size, size_style);
}

void AABBVariantConverter::emit_as_sequence(ryml::NodeRef &node, const AABB &aabb, const YAMLStyle::View &style) const {
	node |= ryml::SEQ;

	// Flow style
	style.apply_flow_style(node);

	// Pass child styles down to nested converters using numeric indices
	YAMLStyle::View position_style = style.is_valid() ? style.get_child("0") : YAMLStyle::View();
	YAMLStyle::View size_style = style.is_valid() ? style.get_child("1") : YAMLStyle::View();

	ryml::NodeRef position_node = node.append_child();
	vec3_converter->encode(position_node, aabb.position, position_style);

	ryml::NodeRef size_node = node.append_child();
	vec3_converter->encode(size_node, aabb.size, size_style);
}

Variant AABBVariantConverter::decode(const ryml::ConstNodeRef &node) const {
	if (node.is_map()) {
		return decode_from_map(node);
	} else if (node.is_seq()) {
		return decode_from_sequence(node);
	}
	throw YAMLException("Invalid AABB format: expected map or sequence");
}

Variant AABBVariantConverter::decode_from_map(const ryml::ConstNodeRef &node) const {
	const auto position_node = node["position"];
	const auto size_node = node["size"];

	if (position_node.invalid()) {
		throw YAMLException("Missing required field 'position' in AABB");
	}

	if (size_node.invalid()) {
		throw YAMLException("Missing required field 'size' in AABB");
	}

	const Vector3 position = vec3_converter->decode(position_node).operator Vector3();
	const Vector3 size = vec3_converter->decode(size_node).operator Vector3();

	return AABB(position, size);
}

Variant AABBVariantConverter::decode_from_sequence(const ryml::ConstNodeRef &node) const {
	if (node.num_children() != 2) {
		throw YAMLException("Invalid AABB sequence: expected exactly 2 elements");
	}

	const Vector3 position = vec3_converter->decode(node[0]).operator Vector3();
	const Vector3 size = vec3_converter->decode(node[1]).operator Vector3();

	return AABB(position, size);
}

} // namespace godot

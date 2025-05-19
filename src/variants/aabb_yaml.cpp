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

	style.apply_flow_style(node);

	ryml::NodeRef position_node;
	ryml::NodeRef size_node;

	if (!style.is_valid() || style.get_container_form() != YAMLStyle::FORM_ARRAY) {
		node |= ryml::MAP;
		position_node = node["position"];
		size_node = node["size"];
	} else {
		node |= ryml::SEQ;
		position_node = node.append_child();
		size_node = node.append_child();
	}

	vec3_converter->encode(position_node, aabb.position, style.has_child("position") ? style.get_child("position") : YAMLStyle::View());
	vec3_converter->encode(size_node, aabb.size, style.has_child("size") ? style.get_child("size") : YAMLStyle::View());
}

Variant AABBVariantConverter::decode(const ryml::ConstNodeRef &node, ParserContext *context) const {
	try {
		if (node.is_map()) {
			return decode_from_map(node, context);
		}

		if (node.is_seq()) {
			return decode_from_sequence(node, context);
		}

		throw create_invalid_format_exception(node);
	} catch (const YAMLException &) {
		throw; // Re-throw YAML exceptions
	} catch (const std::exception &e) {
		throw create_decode_error_exception(e.what(), node);
	}
}

Variant AABBVariantConverter::decode_from_map(const ryml::ConstNodeRef &node, ParserContext *context) const {
	check_required_fields(node, { "position", "size" });

	const bool detect_style = context->detect_style;

	if (detect_style) {
		Ref<YAMLStyle> style = context->current_style();
		YAMLStyle::detect_flow_style(node, style);
		style->set_container_form(YAMLStyle::FORM_DICTIONARY);

		context->push_style("position");
	}

	Vector3 position = vec3_converter->decode(node["position"], context).operator Vector3();

	if (detect_style) {
		context->pop_style();
		context->push_style("size");
	}

	Vector3 size = vec3_converter->decode(node["size"], context).operator Vector3();

	if (detect_style) {
		context->pop_style();
	}

	return AABB(position, size);
}

Variant AABBVariantConverter::decode_from_sequence(const ryml::ConstNodeRef &node, ParserContext *context) const {
	if (node.num_children() != 2) {
		throw create_invalid_sequence_length_exception(2, node);
	}

	const bool detect_style = context->detect_style;

	if (detect_style) {
		Ref<YAMLStyle> style = context->current_style();
		YAMLStyle::detect_flow_style(node, style);
		style->set_container_form(YAMLStyle::FORM_ARRAY);

		context->push_style("position");
	}

	Vector3 position = vec3_converter->decode(node[0], context).operator Vector3();

	if (detect_style) {
		context->pop_style();
		context->push_style("size");
	}

	Vector3 size = vec3_converter->decode(node[1], context).operator Vector3();

	if (detect_style) {
		context->pop_style();
	}

	return AABB(position, size);
}

} // namespace godot

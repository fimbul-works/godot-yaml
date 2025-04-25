#include "rect2_yaml.h"
#include "../converter_factory.h"
#include "../exception.h"
#include "../util_numeric.h"

using namespace godot;

Rect2VariantConverter::Rect2VariantConverter(ConverterFactory *factory) :
		vec2_converter(factory->create_converter_as<Vector2VariantConverter>(Variant::VECTOR2)) {
	ERR_FAIL_NULL(vec2_converter);
}

void Rect2VariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const Rect2 rect = v.operator Rect2();

	if (!style.is_valid() || style.get_container_form() != YAMLStyle::FORM_SEQ) {
		emit_as_map(node, rect, style);
	} else {
		emit_as_sequence(node, rect, style);
	}
}

void Rect2VariantConverter::emit_as_map(ryml::NodeRef &node, const Rect2 &rect, const YAMLStyle::View &style) const {
	node |= ryml::MAP;

	style.apply_flow_style(node);

	YAMLStyle::View pos_style = style.is_valid() ? style.get_child("position") : YAMLStyle::View();
	YAMLStyle::View size_style = style.is_valid() ? style.get_child("size") : YAMLStyle::View();

	ryml::NodeRef pos_node = node["position"];
	vec2_converter->encode(pos_node, rect.position, pos_style);

	ryml::NodeRef size_node = node["size"];
	vec2_converter->encode(size_node, rect.size, size_style);
}

void Rect2VariantConverter::emit_as_sequence(ryml::NodeRef &node, const Rect2 &rect, const YAMLStyle::View &style) const {
	node |= ryml::SEQ;

	style.apply_flow_style(node);

	YAMLStyle::View pos_style = style.is_valid() ? style.get_child("0") : YAMLStyle::View();
	YAMLStyle::View size_style = style.is_valid() ? style.get_child("1") : YAMLStyle::View();

	ryml::NodeRef pos_node = node.append_child();
	vec2_converter->encode(pos_node, rect.position, pos_style);

	ryml::NodeRef size_node = node.append_child();
	vec2_converter->encode(size_node, rect.size, size_style);
}

Variant Rect2VariantConverter::decode(const ryml::ConstNodeRef &node) const {
	try {
		if (node.is_map()) {
			return decode_from_map(node);
		}

		if (node.is_seq()) {
			return decode_from_sequence(node);
		}

		throw create_invalid_format_exception("Rect2", node);
	} catch (const YAMLException &) {
		throw; // Re-throw YAML exceptions
	} catch (const std::exception &e) {
		throw create_decode_error_exception("Rect2", e.what(), node);
	}
}

Variant Rect2VariantConverter::decode_from_map(const ryml::ConstNodeRef &node) const {
	check_required_fields(node, { "position", "size" });

	Vector2 position = vec2_converter->decode(node["position"]).operator Vector2();
	Vector2 size = vec2_converter->decode(node["size"]).operator Vector2();

	return Rect2(position, size);
}

Variant Rect2VariantConverter::decode_from_sequence(const ryml::ConstNodeRef &node) const {
	if (node.num_children() != 2) {
		throw create_invalid_sequence_length_exception("Rect2", 2, node);
	}

	Vector2 position = vec2_converter->decode(node[0]).operator Vector2();
	Vector2 size = vec2_converter->decode(node[1]).operator Vector2();

	return Rect2(position, size);
}

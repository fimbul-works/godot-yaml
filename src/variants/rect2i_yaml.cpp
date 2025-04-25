#include "rect2i_yaml.h"
#include "../converter_factory.h"
#include "../exception.h"
#include "../util_numeric.h"

using namespace godot;

Rect2iVariantConverter::Rect2iVariantConverter(ConverterFactory *factory) :
		vec2i_converter(factory->create_converter_as<Vector2iVariantConverter>(Variant::VECTOR2I)) {
	ERR_FAIL_NULL(vec2i_converter);
}

void Rect2iVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const Rect2i rect = v.operator Rect2i();

	if (!style.is_valid() || style.get_container_form() != YAMLStyle::FORM_SEQ) {
		emit_as_map(node, rect, style);
	} else {
		emit_as_sequence(node, rect, style);
	}
}

void Rect2iVariantConverter::emit_as_map(ryml::NodeRef &node, const Rect2i &rect, const YAMLStyle::View &style) const {
	node |= ryml::MAP;

	style.apply_flow_style(node);

	YAMLStyle::View pos_style = style.is_valid() ? style.get_child("position") : YAMLStyle::View();
	YAMLStyle::View size_style = style.is_valid() ? style.get_child("size") : YAMLStyle::View();

	ryml::NodeRef pos_node = node["position"];
	vec2i_converter->encode(pos_node, rect.position, pos_style);

	ryml::NodeRef size_node = node["size"];
	vec2i_converter->encode(size_node, rect.size, size_style);
}

void Rect2iVariantConverter::emit_as_sequence(ryml::NodeRef &node, const Rect2i &rect, const YAMLStyle::View &style) const {
	node |= ryml::SEQ;

	style.apply_flow_style(node);

	YAMLStyle::View pos_style = style.is_valid() ? style.get_child("0") : YAMLStyle::View();
	YAMLStyle::View size_style = style.is_valid() ? style.get_child("1") : YAMLStyle::View();

	ryml::NodeRef pos_node = node.append_child();
	vec2i_converter->encode(pos_node, rect.position, pos_style);

	ryml::NodeRef size_node = node.append_child();
	vec2i_converter->encode(size_node, rect.size, size_style);
}

Variant Rect2iVariantConverter::decode(const ryml::ConstNodeRef &node) const {
	try {
		if (node.is_map()) {
			return decode_from_map(node);
		}

		if (node.is_seq()) {
			return decode_from_sequence(node);
		}

		throw YAMLException::create_invalid_format("Rect2i");
	} catch (const YAMLException &) {
		throw; // Re-throw YAML exceptions
	} catch (const std::exception &e) {
		throw YAMLException::create_decode_error("Rect2i", e.what());
	}
}

Variant Rect2iVariantConverter::decode_from_map(const ryml::ConstNodeRef &node) const {
	check_required_fields(node, { "position", "size" });

	Vector2i position = vec2i_converter->decode(node["position"]).operator Vector2i();
	Vector2i size = vec2i_converter->decode(node["size"]).operator Vector2i();

	return Rect2i(position, size);
}

Variant Rect2iVariantConverter::decode_from_sequence(const ryml::ConstNodeRef &node) const {
	if (node.num_children() != 2) {
		throw YAMLException::create_invalid_sequence_length("Rect2i", 2);
	}

	Vector2i position = vec2i_converter->decode(node[0]).operator Vector2i();
	Vector2i size = vec2i_converter->decode(node[1]).operator Vector2i();

	return Rect2i(position, size);
}

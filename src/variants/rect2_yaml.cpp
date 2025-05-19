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

	auto scalar_style = style.get_scalar_view();
	vec2_converter->encode(position_node, rect.position, style.has_child("position") ? style.get_child("position") : scalar_style);
	vec2_converter->encode(size_node, rect.size, style.has_child("size") ? style.get_child("size") : scalar_style);
}

Variant Rect2VariantConverter::decode(const ryml::ConstNodeRef &node, ParserContext *context) const {
	try {
		if (node.is_map()) {
			return decode_from_map(node, context);
		}

		if (node.is_seq()) {
			return decode_from_sequence(node, context);
		}

		throw create_invalid_format_exception(node);
	} catch (const YAMLException &e) {
		throw YAMLException(vformat("Failed to decode Rect2: %s", e.what()), e.get_location());
	} catch (const std::exception &e) {
		throw create_decode_error_exception(e.what(), node);
	}
}

Rect2 Rect2VariantConverter::decode_from_map(const ryml::ConstNodeRef &node, ParserContext *context) const {
	check_required_fields(node, { "position", "size" });

	const bool detect_style = context->detect_style;

	if (detect_style) {
		Ref<YAMLStyle> style = context->current_style();
		YAMLStyle::detect_flow_style(node, style);
		style->set_container_form(YAMLStyle::FORM_DICTIONARY);

		context->push_style("position");
	}

	Vector2 position = vec2_converter->decode(node["position"], context).operator Vector2();

	if (detect_style) {
		context->pop_style();
		context->push_style("size");
	}

	Vector2 size = vec2_converter->decode(node["size"], context).operator Vector2();

	if (detect_style) {
		context->pop_style();
	}

	return Rect2(position, size);
}

Rect2 Rect2VariantConverter::decode_from_sequence(const ryml::ConstNodeRef &node, ParserContext *context) const {
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

	Vector2 position = vec2_converter->decode(node[0], context).operator Vector2();

	if (detect_style) {
		context->pop_style();
		context->push_style("size");
	}

	Vector2 size = vec2_converter->decode(node[1], context).operator Vector2();

	if (detect_style) {
		context->pop_style();
	}

	return Rect2(position, size);
}

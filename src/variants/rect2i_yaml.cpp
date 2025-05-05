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

	style.apply_flow_style(node);

	if (!style.is_valid() || style.get_container_form() != YAMLStyle::FORM_ARRAY) {
		node |= ryml::MAP;
		vec2i_converter->encode(node["position"], rect.position, style.get_child("position"));
		vec2i_converter->encode(node["size"], rect.size, style.get_child("size"));
	} else {
		node |= ryml::SEQ;
		vec2i_converter->encode(node.append_child(), rect.position, style.get_child("position"));
		vec2i_converter->encode(node.append_child(), rect.size, style.get_child("size"));
	}
}

Variant Rect2iVariantConverter::decode(const ryml::ConstNodeRef &node, ParserContext *context) const {
	try {
		if (node.is_map()) {
			return decode_from_map(node, context);
		}

		if (node.is_seq()) {
			return decode_from_sequence(node, context);
		}

		throw create_invalid_format_exception(node);
	} catch (const YAMLException &e) {
		throw YAMLException(vformat("Failed to decode Rect2i: %s", e.what()), e.get_location());
	} catch (const std::exception &e) {
		throw create_decode_error_exception(e.what(), node);
	}
}

Rect2i Rect2iVariantConverter::decode_from_map(const ryml::ConstNodeRef &node, ParserContext *context) const {
	check_required_fields(node, { "position", "size" });

	const bool detect_style = context->detect_style;

	if (detect_style) {
		Ref<YAMLStyle> style = context->current_style();
		YAMLStyle::detect_flow_style(node, style);
		style->set_container_form(YAMLStyle::FORM_DICTIONARY);

		context->push_style("position");
	}

	Vector2i position = vec2i_converter->decode(node["position"], context).operator Vector2i();

	if (detect_style) {
		context->pop_style();
		context->push_style("size");
	}

	Vector2i size = vec2i_converter->decode(node["size"], context).operator Vector2i();

	if (detect_style) {
		context->pop_style();
	}

	return Rect2i(position, size);
}

Rect2i Rect2iVariantConverter::decode_from_sequence(const ryml::ConstNodeRef &node, ParserContext *context) const {
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

	Vector2i position = vec2i_converter->decode(node[0], context).operator Vector2i();

	if (detect_style) {
		context->pop_style();
		context->push_style("size");
	}

	Vector2i size = vec2i_converter->decode(node[1], context).operator Vector2i();

	if (detect_style) {
		context->pop_style();
	}

	return Rect2i(position, size);
}

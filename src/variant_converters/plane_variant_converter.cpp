#include "plane_variant_converter.hpp"
#include "../exception.hpp"
#include "../util/util_numeric.hpp"
#include "variant_converter_factory.hpp"

using namespace godot;

YAMLPlaneVariantConverter::YAMLPlaneVariantConverter(YAMLVariantConverterFactory *factory) :
		vec3_converter(factory->create_converter_as<YAMLVector3VariantConverter>(Variant::VECTOR3)) {
	ERR_FAIL_NULL(vec3_converter);
}

void YAMLPlaneVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const Plane plane = v.operator Plane();

	style.apply_flow_style(node);

	ryml::NodeRef normal_node;
	ryml::NodeRef d_node;

	if (!style.is_valid() || style.get_container_form() != YAMLStyle::FORM_ARRAY) {
		node |= ryml::MAP;
		normal_node = node["normal"];
		d_node = node["d"];
	} else {
		node |= ryml::SEQ;
		normal_node = node.append_child();
		d_node = node.append_child();
	}

	vec3_converter->encode(normal_node, plane.normal, style.has_child("normal") ? style.get_child("normal") : style.get_scalar_view());
	d_node << float_to_string(plane.d, style.has_child("d") ? style.get_child("d").get_float_format() : YAMLStyle::FLOAT_ANY);
}

Variant YAMLPlaneVariantConverter::decode(const ryml::ConstNodeRef &node, YAMLParserContext *context) const {
	try {
		if (node.is_map()) {
			return decode_from_map(node, context);
		}

		if (node.is_seq()) {
			return decode_from_sequence(node, context);
		}

		throw create_invalid_format_exception(node);
	} catch (const YAMLException &e) {
		throw YAMLException(vformat("Failed to decode Plane: %s", e.what()), e.get_location());
	} catch (const std::exception &e) {
		throw create_decode_error_exception(e.what(), node);
	}
}

Variant YAMLPlaneVariantConverter::decode_from_map(const ryml::ConstNodeRef &node, YAMLParserContext *context) const {
	check_required_fields(node, { "normal", "d" });

	const bool detect_style = context->detect_style;

	if (detect_style) {
		Ref<YAMLStyle> style = context->current_style();
		YAMLStyle::detect_flow_style(node, style);
		style->set_container_form(YAMLStyle::FORM_DICTIONARY);

		context->push_style("normal");
	}

	Vector3 normal = vec3_converter->decode(node["normal"], context).operator Vector3();

	if (detect_style) {
		context->pop_style();
		context->push_style("d");
	}

	YAMLStyle::FloatFormat d_format;
	real_t d = string_to_float<real_t>(node["d"].val(), detect_style ? &d_format : nullptr);

	if (detect_style) {
		context->current_style()->set_float_format(d_format);
		context->pop_style();
	}

	return Plane(normal, d);
}

Variant YAMLPlaneVariantConverter::decode_from_sequence(const ryml::ConstNodeRef &node, YAMLParserContext *context) const {
	if (node.num_children() != 2) {
		throw create_invalid_sequence_length_exception(2, node);
	}

	const bool detect_style = context->detect_style;

	if (detect_style) {
		Ref<YAMLStyle> style = context->current_style();
		YAMLStyle::detect_flow_style(node, style);
		style->set_container_form(YAMLStyle::FORM_ARRAY);

		context->push_style("normal");
	}

	Vector3 normal = vec3_converter->decode(node[0], context).operator Vector3();

	if (detect_style) {
		context->pop_style();
		context->push_style("d");
	}

	YAMLStyle::FloatFormat d_format;
	real_t d = string_to_float<real_t>(node[1].val(), detect_style ? &d_format : nullptr);

	if (detect_style) {
		context->current_style()->set_float_format(d_format);
		context->pop_style();
	}

	return Plane(normal, d);
}

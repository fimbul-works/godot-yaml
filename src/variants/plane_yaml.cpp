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

	style.apply_flow_style(node);

	if (!style.is_valid() || style.get_container_form() != YAMLStyle::FORM_SEQ) {
		node |= ryml::MAP;
		vec3_converter->encode(node["normal"], plane.normal, style.get_child("normal"));
		node["d"] << float_to_string(plane.d, style.get_child("d").get_float_format());
	} else {
		node |= ryml::SEQ;
		vec3_converter->encode(node.append_child(), plane.normal, style.get_child("normal"));
		node.append_child() << float_to_string(plane.d, style.get_child("d").get_float_format());
	}
}

Variant PlaneVariantConverter::decode(const ryml::ConstNodeRef &node, ParserContext *context) const {
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

Variant PlaneVariantConverter::decode_from_map(const ryml::ConstNodeRef &node, ParserContext *context) const {
	check_required_fields(node, { "normal", "d" });

	const bool detect_style = context->detect_style;

	if (detect_style) {
		Ref<YAMLStyle> style = context->current_style();
		YAMLStyle::detect_flow_style(node, style);
		style->set_container_form(YAMLStyle::FORM_MAP);

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

Variant PlaneVariantConverter::decode_from_sequence(const ryml::ConstNodeRef &node, ParserContext *context) const {
	if (node.num_children() != 2) {
		throw create_invalid_sequence_length_exception(2, node);
	}

	const bool detect_style = context->detect_style;

	if (detect_style) {
		Ref<YAMLStyle> style = context->current_style();
		YAMLStyle::detect_flow_style(node, style);
		style->set_container_form(YAMLStyle::FORM_SEQ);

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

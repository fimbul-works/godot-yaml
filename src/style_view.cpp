#include "style_view.h"
#include "util_string.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <ryml.hpp>

using namespace godot;

// YAMLStyleView implementation
YAMLStyle::View::View(std::shared_ptr<const ViewData> p_data) :
		data(std::move(p_data)) {
}

YAMLStyle::View YAMLStyle::View::create_view(const Ref<YAMLStyle> &style) {
	if (!style.is_valid()) {
		return YAMLStyle::View();
	}

	auto view_data = std::make_shared<ViewData>();
	if (!view_data) {
		return View();
	}

	// Copy all style values
	view_data->has_container_form = style->has_container_form;
	view_data->container_form = style->container_form;

	view_data->has_flow_style = style->has_flow_style;
	view_data->flow_style = style->flow_style;

	view_data->has_string_style = style->has_string_style;
	view_data->string_style = style->string_style;

	view_data->has_integer_format = style->has_integer_format;
	view_data->integer_format = style->integer_format;

	view_data->has_float_format = style->has_float_format;
	view_data->float_format = style->float_format;

	view_data->has_binary_encoding = style->has_binary_encoding;
	view_data->binary_encoding = style->binary_encoding;

	view_data->has_custom_settings = style->has_custom_settings;
	view_data->custom_settings = style->custom_settings;

	// Create views for all children
	Array keys = style->get_children_keys();
	for (int i = 0; i < keys.size(); i++) {
		String key = keys[i];
		if (key.is_empty()) {
			continue;
		}

		Ref<YAMLStyle> child = style->get_child(key);
		if (child.is_valid()) {
			auto child_view = create_view(child);
			if (child_view.is_valid()) {
				view_data->children[key] = child_view.data;
			}
		}
	}

	return YAMLStyle::View(std::move(view_data));
}

// YAMLStyleView accessors
YAMLStyle::ContainerForm YAMLStyle::View::get_container_form() const {
	if (data && data->has_container_form) {
		return data->container_form;
	}

	auto template_style = get_template_style();
	if (template_style.is_valid()) {
		return template_style.get_container_form();
	}

	return YAMLStyle::FORM_ANY;
}

YAMLStyle::FlowStyle YAMLStyle::View::get_flow_style() const {
	if (data && data->has_flow_style) {
		return data->flow_style;
	}

	auto template_style = get_template_style();
	if (template_style.is_valid()) {
		return template_style.get_flow_style();
	}

	return YAMLStyle::FLOW_ANY;
}

YAMLStyle::StringStyle YAMLStyle::View::get_string_style() const {
	if (data && data->has_string_style) {
		return data->string_style;
	}

	auto template_style = get_template_style();
	if (template_style.is_valid()) {
		return template_style.get_string_style();
	}

	return YAMLStyle::STRING_ANY;
}

YAMLStyle::IntegerFormat YAMLStyle::View::get_integer_format() const {
	if (data && data->has_integer_format) {
		return data->integer_format;
	}

	auto template_style = get_template_style();
	if (template_style.is_valid()) {
		return template_style.get_integer_format();
	}

	return YAMLStyle::INT_ANY;
}

YAMLStyle::FloatFormat YAMLStyle::View::get_float_format() const {
	if (data && data->has_float_format) {
		return data->float_format;
	}

	auto template_style = get_template_style();
	if (template_style.is_valid()) {
		return template_style.get_float_format();
	}

	return YAMLStyle::FLOAT_ANY;
}

YAMLStyle::BinaryEncoding YAMLStyle::View::get_binary_encoding() const {
	if (data && data->has_binary_encoding) {
		return data->binary_encoding;
	}

	auto template_style = get_template_style();
	if (template_style.is_valid()) {
		return template_style.get_binary_encoding();
	}

	return YAMLStyle::BIN_ANY;
}

YAMLStyle::View YAMLStyle::View::get_template_style() const {
	if (data && data->children.count("_template")) {
		return View(data->children.at("_template"));
	}
	return View();
}

bool YAMLStyle::View::is_block_style() const {
	return get_string_style() == YAMLStyle::STRING_LITERAL || get_string_style() == YAMLStyle::STRING_FOLDED;
}

bool YAMLStyle::View::uses_quotes() const {
	return get_string_style() == YAMLStyle::STRING_QUOTE_SINGLE || get_string_style() == YAMLStyle::STRING_QUOTE_DOUBLE;
}

bool YAMLStyle::View::uses_flow() const {
	return get_flow_style() == YAMLStyle::FLOW_SINGLE;
}

void YAMLStyle::View::apply_flow_style(ryml::NodeRef &node) const {
	if (!is_valid()) {
		return;
	}

	if (data->flow_style == YAMLStyle::FLOW_SINGLE) {
		node |= ryml::FLOW_SL;
	}
}

void YAMLStyle::View::apply_string_style(ryml::NodeRef &node) const {
	if (!is_valid()) {
		return;
	}

	switch (data->string_style) {
		case YAMLStyle::STRING_QUOTE_SINGLE:
			node |= ryml::VAL_SQUO;
			break;
		case YAMLStyle::STRING_QUOTE_DOUBLE:
			node |= ryml::VAL_DQUO;
			break;
		case YAMLStyle::STRING_LITERAL:
			node |= ryml::VAL_LITERAL | ryml::BLOCK;
			break;
		case YAMLStyle::STRING_FOLDED:
			node |= ryml::VAL_FOLDED | ryml::BLOCK;
			break;
		default:
			break;
	}
}

bool YAMLStyle::View::is_valid() const {
	return data != nullptr;
}

YAMLStyle::View YAMLStyle::View::get_child(const String &key) const {
	if (data && data->children.count(key)) {
		return YAMLStyle::View(data->children.at(key));
	}

	// If no specific child style exists, return the current style
	return *this;
}

bool YAMLStyle::View::has_child(const String &key) const {
	return data && data->children.count(key);
}

const Dictionary &YAMLStyle::View::get_custom_settings() const {
	static const Dictionary empty;
	return data ? data->custom_settings : empty;
}

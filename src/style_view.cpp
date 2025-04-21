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
	view_data->has_scalar_style = style->has_scalar_style;
	view_data->scalar_style = style->scalar_style;

	view_data->has_quote_style = style->has_quote_style;
	view_data->quote_style = style->quote_style;

	view_data->has_container_form = style->has_container_form;
	view_data->container_form = style->container_form;

	view_data->has_flow_style = style->has_flow_style;
	view_data->flow_style = style->flow_style;

	view_data->has_number_format = style->has_number_format;
	view_data->number_format = style->number_format;

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
YAMLStyle::ScalarStyle YAMLStyle::View::get_scalar_style() const {
	if (data && data->has_scalar_style) {
		return data->scalar_style;
	}

	auto template_style = get_template_style();
	if (template_style.is_valid()) {
		return template_style.get_scalar_style();
	}

	return YAMLStyle::SCALAR_ANY;
}

YAMLStyle::QuoteStyle YAMLStyle::View::get_quote_style() const {
	if (data && data->has_quote_style) {
		return data->quote_style;
	}

	auto template_style = get_template_style();
	if (template_style.is_valid()) {
		return template_style.get_quote_style();
	}

	return YAMLStyle::QUOTE_ANY;
}

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

YAMLStyle::NumberFormat YAMLStyle::View::get_number_format() const {
	if (data && data->has_number_format) {
		return data->number_format;
	}

	auto template_style = get_template_style();
	if (template_style.is_valid()) {
		return template_style.get_number_format();
	}

	return YAMLStyle::NUM_ANY;
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

// Helper methods
bool YAMLStyle::View::is_block_style() const {
	return get_scalar_style() == YAMLStyle::SCALAR_BLOCK || get_scalar_style() == YAMLStyle::SCALAR_LITERAL || get_scalar_style() == YAMLStyle::SCALAR_FOLDED;
}

bool YAMLStyle::View::uses_quotes() const {
	return get_quote_style() == YAMLStyle::QUOTE_SINGLE || get_quote_style() == YAMLStyle::QUOTE_DOUBLE;
}

bool YAMLStyle::View::uses_flow() const {
	return get_flow_style() == YAMLStyle::FLOW_SINGLE;
}

void YAMLStyle::View::apply_scalar_style(ryml::NodeRef &node) const {
	// Only apply styles when requested
	if (!is_valid()) {
		return;
	}

	// Scalar styles
	if (data->scalar_style != YAMLStyle::SCALAR_ANY) {
		switch (data->scalar_style) {
			case YAMLStyle::SCALAR_BLOCK:
				node |= ryml::BLOCK;
				break;
			case YAMLStyle::SCALAR_LITERAL:
				node |= ryml::VAL_LITERAL | ryml::BLOCK;
				break;
			case YAMLStyle::SCALAR_FOLDED:
				node |= ryml::VAL_FOLDED;
				break;
			case YAMLStyle::SCALAR_PLAIN:
				break;
			default:
				break;
		}
	}
}

void YAMLStyle::View::apply_quote_style(ryml::NodeRef &node) const {
	// Only apply styles when requested
	if (!is_valid()) {
		return;
	}

	// Handle quote style - this can be combined with scalar style
	if (data->quote_style != YAMLStyle::QUOTE_ANY) {
		// User explicitly specified quote style
		switch (data->quote_style) {
			case YAMLStyle::QUOTE_SINGLE:
				node |= ryml::VAL_SQUO;
				break;
			case YAMLStyle::QUOTE_DOUBLE:
				node |= ryml::VAL_DQUO;
				break;
			case YAMLStyle::QUOTE_NONE:
				// No quotes - no action needed
				break;
			default:
				break;
		}
	}
}

void YAMLStyle::View::apply_flow_style(ryml::NodeRef &node) const {
	if (!is_valid()) {
		return;
	}

	if (data->flow_style == YAMLStyle::FLOW_SINGLE) {
		node |= ryml::FLOW_SL;
	}
}

bool YAMLStyle::View::is_valid() const {
	return data != nullptr;
}

// Child management
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

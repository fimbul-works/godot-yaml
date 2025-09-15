#include "style_view.hpp"
#include "util_string.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <ryml.hpp>

using namespace godot;

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
	view_data->container_form = style->container_form;
	view_data->flow_style = style->flow_style;
	view_data->string_style = style->string_style;
	view_data->integer_format = style->integer_format;
	view_data->float_format = style->float_format;

	view_data->custom_settings = style->custom_settings.duplicate(true);

	// Create views for all children
	Array keys = style->list_children();
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

YAMLStyle::View YAMLStyle::View::get_scalar_view() const {
	if (data) {
		auto scalar_view_data = std::make_shared<ViewData>();

		if (scalar_view_data) {
			scalar_view_data->container_form = data->container_form;
			scalar_view_data->flow_style = data->flow_style;
			scalar_view_data->string_style = data->string_style;
			scalar_view_data->integer_format = data->integer_format;
			scalar_view_data->float_format = data->float_format;

			return YAMLStyle::View(std::move(scalar_view_data));
		}
	}

	return YAMLStyle::View();
}

YAMLStyle::ContainerForm YAMLStyle::View::get_container_form() const {
	return data ? data->container_form : YAMLStyle::FORM_ANY;
}

YAMLStyle::FlowStyle YAMLStyle::View::get_flow_style() const {
	return data ? data->flow_style : YAMLStyle::FLOW_ANY;
}

YAMLStyle::StringStyle YAMLStyle::View::get_string_style() const {
	return data ? data->string_style : YAMLStyle::STRING_ANY;
}

YAMLStyle::IntegerFormat YAMLStyle::View::get_integer_format() const {
	return data ? data->integer_format : YAMLStyle::INT_ANY;
}

YAMLStyle::FloatFormat YAMLStyle::View::get_float_format() const {
	return data ? data->float_format : YAMLStyle::FLOAT_ANY;
}

String YAMLStyle::View::get_custom_tag() const {
	if (data && data->custom_settings.has("tag")) {
		return data->custom_settings["tag"];
	}
	return "";
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

	// If no specific child style exists, return scalar styles
	return get_scalar_view();
}

bool YAMLStyle::View::has_child(const String &key) const {
	return data && data->children.count(key);
}

const Dictionary &YAMLStyle::View::get_custom_settings() const {
	static const Dictionary empty;
	return data ? data->custom_settings : empty;
}

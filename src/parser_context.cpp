#include "parser_context.hpp"

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

YAMLParserContext::YAMLParserContext(const ryml::Parser *p_ryml_parser, const Ref<YAMLStyle> &root_style) :
		ryml_parser(p_ryml_parser), detect_style(root_style.is_valid()) {
	if (detect_style) {
		style_stack.push(root_style);
	}
}

Ref<YAMLStyle> YAMLParserContext::current_style() const {
	if (style_stack.empty()) {
		throw YAMLException("Style stack is empty!");
	}

	Ref<YAMLStyle> current_style = style_stack.top();
	if (!current_style.is_valid()) {
		throw YAMLException("Current style is not valid!");
	}

	return current_style;
}

Ref<YAMLStyle> YAMLParserContext::push_style(const String &key) {
	if (!detect_style) {
		return Ref<YAMLStyle>();
	}

	Ref<YAMLStyle> parent = style_stack.top();
	if (!parent.is_valid()) {
		throw new YAMLException("Not a valid YAMLStyle");
	}

	Ref<YAMLStyle> child;
	child.instantiate();
	parent->set_child(key, child);
	style_stack.push(child);
	return child;
}

void YAMLParserContext::pop_style() {
	if (!detect_style || style_stack.empty()) {
		return;
	}

	style_stack.pop();
}

const ryml::Parser *YAMLParserContext::get_ryml_parser() const {
	return ryml_parser;
}

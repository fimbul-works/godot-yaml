#ifndef YAML_PARSER_CONTEXT_H
#define YAML_PARSER_CONTEXT_H

#include "exception.h"
#include "style.h"
#include "yaml.h"

#include <ryml.hpp>

#include <memory>
#include <stack>

namespace godot {

class ParserContext {
public:
	ParserContext(const ryml::Parser *p_ryml_parser, const Ref<YAMLStyle> &root_style = nullptr);
	~ParserContext() = default;

	Ref<YAMLStyle> current_style() const;
	Ref<YAMLStyle> push_style(const String &key);
	void pop_style();
	const ryml::Parser *get_ryml_parser() const;

	const bool detect_style;

private:
	const ryml::Parser *ryml_parser;
	std::stack<Ref<YAMLStyle>> style_stack;
};

} // namespace godot

#endif // YAML_PARSER_H

#ifndef YAML_PARSER_H
#define YAML_PARSER_H

#include "converter_factory.h"
#include "parser_context.h"
#include "result.h"
#include "security.h"
#include "string_hash.h"
#include "style.h"
#include "yaml.h"

#include <ryml.hpp>

#include <memory>
#include <optional>
#include <stack>

namespace godot {

class YAML::Parser {
public:
	Parser();
	~Parser() = default;

	// Non-copyable
	Parser(const Parser &) = delete;
	Parser &operator=(const Parser &) = delete;

	// Parse YAML input string
	Ref<YAMLResult> parse(const String &input, const YAMLSecurity::View &security_view = YAMLSecurity::get_default_view(), const bool detect_style = false);

private:
	// Parser components
	ryml::Tree tree;
	ryml::Callbacks callbacks;
	std::unique_ptr<ryml::EventHandlerTree> evt_handler;
	std::unique_ptr<ryml::Parser> ryml_parser;

	// Owned converters
	ConverterFactory factory;
	std::unordered_map<Variant::Type, std::unique_ptr<VariantConverter>> type_converters;
	std::unordered_map<String, VariantConverter *, StringHasher, StringEqual> tag_converters;

	// Current parse state
	Ref<YAMLResult> current_result;
	std::unique_ptr<ParserContext> context;
	Ref<YAMLStyle> style;
	bool detect_style = false;

	// Error handling
	static void error_callback(const char *msg, size_t len, ryml::Location loc, void *user_data);

	// Node processing methods
	Variant process_node(const ryml::ConstNodeRef &node) const;
	Variant process_common(const ryml::ConstNodeRef &node) const;
	Variant process_map(const ryml::ConstNodeRef &node) const;
	Variant process_sequence(const ryml::ConstNodeRef &node) const;
	Variant process_value(const ryml::ConstNodeRef &node) const;

	// Value parsing helpers
	std::optional<Variant> try_parse_tagged_value(const ryml::ConstNodeRef &node) const;
	std::optional<Variant> try_parse_special_value(const String &str_val) const;
	std::optional<Variant> try_parse_numeric_value(const String &str_val, const ryml::csubstr &val) const;
	String extract_tag(const ryml::ConstNodeRef &node) const;
	String extract_key(const ryml::ConstNodeRef &node) const;

	// Object type handlers
	Variant parse_object_or_resource(const ryml::ConstNodeRef &node, const String &class_name) const;
	Variant load_resource(const String &path, const ryml::ConstNodeRef &node) const;
	void populate_object_properties(Object *obj, const ryml::ConstNodeRef &node) const;

	// Initialize converters
	void init_converters();

	// Converter access helpers
	VariantConverter *get_converter_for_type(Variant::Type type) const;
	VariantConverter *get_converter_for_tag(const String &tag) const;

	// Current path style helper
	Ref<YAMLStyle> get_current_path_style();

	// Security view for resource access
	YAMLSecurity::View security_view;
};

} // namespace godot

#endif // YAML_PARSER_H

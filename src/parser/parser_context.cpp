#include "parser_context.hpp"

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

YAMLParserContext::YAMLParserContext(const ryml::Parser *p_ryml_parser, const Ref<YAMLStyle> &root_style, const Ref<Schema> &root_schema) :
		ryml_parser(p_ryml_parser), detect_style(root_style.is_valid()), has_root_schema(true) {
	if (detect_style) {
		style_stack.push(root_style);
	}

	if (root_schema.is_valid()) {
		schema_stack.push(root_schema);
	} else {
		// We might need to discover schema from $schema field
		has_root_schema = false;
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

Ref<Schema> YAMLParserContext::resolve_schema_reference(const Ref<Schema> &schema) const {
	if (!schema.is_valid()) {
		return schema;
	}

	Dictionary schema_def = schema->get_schema_definition();

	// Check if this is a $ref schema
	if (schema_def.has("$ref") && schema_def["$ref"].get_type() == Variant::STRING) {
		String ref_uri = schema_def["$ref"];
		Ref<Schema> resolved = schema->resolve_reference(ref_uri);

		if (resolved.is_valid()) {
			return resolved;
		}
	}

	return schema;
}

void YAMLParserContext::push_schema_for_property(const String &property_name) {
	if (!is_validating()) {
		return;
	}

	Ref<Schema> current = current_schema();
	if (!current.is_valid()) {
		return;
	}

	// Try to get property schema
	StringName prop_key = vformat("properties/%s", property_name);
	Ref<Schema> child = current->get_child(prop_key);

	if (child.is_valid()) {
		// Resolve $ref if present so we can access defaults
		child = resolve_schema_reference(child);
		schema_stack.push(child);
	} else {
		// Try additionalProperties
		Ref<Schema> additional = current->get_child("additionalProperties");
		if (additional.is_valid()) {
			additional = resolve_schema_reference(additional);
			schema_stack.push(additional);
		} else {
			// No schema for this property - push null to maintain stack depth
			schema_stack.push(Ref<Schema>());
		}
	}
}

void YAMLParserContext::push_schema_for_array_item(int index) {
	if (!is_validating()) {
		return;
	}

	Ref<Schema> current = current_schema();
	if (!current.is_valid()) {
		return;
	}

	// Try tuple validation (items/0, items/1, ...)
	StringName item_key = vformat("items/%d", index);
	Ref<Schema> child = current->get_child(item_key);

	if (child.is_valid()) {
		child = resolve_schema_reference(child);
		schema_stack.push(child);
		return;
	}

	// Try uniform items schema
	push_schema_for_array_items();
}

void YAMLParserContext::push_schema_for_array_items() {
	if (!is_validating()) {
		return;
	}

	Ref<Schema> current = current_schema();
	if (!current.is_valid()) {
		return;
	}

	Ref<Schema> items_schema = current->get_child("items");
	if (items_schema.is_valid()) {
		items_schema = resolve_schema_reference(items_schema);
		schema_stack.push(items_schema);
	} else {
		schema_stack.push(Ref<Schema>());
	}
}

void YAMLParserContext::pop_schema() {
	if (!schema_stack.empty()) {
		schema_stack.pop();
	}
}

Ref<Schema> YAMLParserContext::current_schema() const {
	if (schema_stack.empty()) {
		return Ref<Schema>();
	}
	return schema_stack.top();
}

void YAMLParserContext::add_validation_error(const ValidationError &error) {
	validation_errors.push_back(error);
}

String YAMLParserContext::get_current_instance_path() const {
	if (instance_path_segments.empty()) {
		return "/";
	}

	String path = "";
	for (const String &segment : instance_path_segments) {
		path += "/" + segment;
	}
	return path;
}

void YAMLParserContext::push_instance_path_segment(const String &segment) {
	instance_path_segments.push_back(segment);
}

void YAMLParserContext::pop_instance_path_segment() {
	if (!instance_path_segments.empty()) {
		instance_path_segments.pop_back();
	}
}

void YAMLParserContext::push_resource_context(const String &resource_path) {
	resource_path_stack.push_back(resource_path);
}

void YAMLParserContext::pop_resource_context() {
	if (!resource_path_stack.empty()) {
		resource_path_stack.pop_back();
	}
}

Array YAMLParserContext::get_resource_path_stack() const {
	Array result;
	for (const String &path : resource_path_stack) {
		result.push_back(path);
	}
	return result;
}

const ryml::Parser *YAMLParserContext::get_ryml_parser() const {
	return ryml_parser;
}

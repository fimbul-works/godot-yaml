#include "parser.h"
#include "class_registry.h"
#include "converter_factory.h"
#include "security.h"
#include "util_numeric.h"

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

YAML::Parser::Parser() {
	callbacks.m_error = error_callback;
	callbacks.m_user_data = this;
	evt_handler = std::make_unique<ryml::EventHandlerTree>(callbacks);
	ryml_parser = std::make_unique<ryml::Parser>(evt_handler.get(), ryml::ParserOptions().locations(true));

	init_converters();
}

void YAML::Parser::init_converters() {
	type_converters = factory.create_converter_set();

	for (const auto &pair : type_converters) {
		if (pair.second) {
			tag_converters[pair.second->get_tag()] = pair.second.get();
		}
	}
}

void YAML::Parser::error_callback(const char *msg, size_t len, ryml::Location loc, void *user_data) {
	ryml::csubstr error_msg(msg, len);

	// Strip "ERROR: " prefix if available
	const ryml::csubstr strip_error_prefix = "ERROR: ";
	if (error_msg.begins_with(strip_error_prefix)) {
		error_msg = error_msg.sub(strip_error_prefix.len);
	}

	// RapidYAML does not like complex keys
	if (error_msg.begins_with("ryml trees cannot handle containers as keys")) {
		error_msg = ryml::to_csubstr("unsupported complex key");
	}

	// Get first line of error message
	size_t newline_pos = error_msg.find('\n');
	if (newline_pos != ryml::substr::npos) {
		error_msg = error_msg.sub(0, newline_pos);
	}

	auto *parser = static_cast<Parser *>(user_data);
	if (!parser) {
		throw YAMLException(from_ryml_str(error_msg), loc);
	}

	parser->current_result = YAMLResult::error(
			from_ryml_str(error_msg),
			loc.line,
			loc.col);

	// Error handler MUST throw!
	throw YAMLException(parser->current_result->get_error_message(), loc);
}

Ref<YAMLResult> YAML::Parser::parse(const String &input, const YAMLSecurity::View &p_security_view, const bool p_detect_style) {
	try {
		current_result = YAMLResult::success(Variant());
		security_view = p_security_view;
		style = YAML::create_style();
		detect_style = p_detect_style;

		ryml::parse_in_arena(
				ryml_parser.get(),
				input.utf8().get_data(),
				&tree);

		if (tree.empty()) {
			return YAMLResult::error("Empty YAML document");
		}

		// Resolve anchors and aliases
		tree.resolve();

		// Disable style detection if multiple documents
		if (tree.rootref().is_stream() && tree.rootref().num_children() > 1) {
			detect_style = false;
		}

		context = std::make_unique<ParserContext>(ryml_parser.get(), detect_style ? style : nullptr);

		if (!current_result->has_error()) {
			if (tree.rootref().is_stream() && tree.rootref().num_children() > 1) {
				Array documents;
				for (const auto &child : tree.rootref().children()) {
					documents.push_back(process_node(child));
				}
				current_result = YAMLResult::success(documents);
			} else {
				Variant parsed_data = process_node(tree.rootref());
				current_result = YAMLResult::success(parsed_data, style);
			}
		}

		context.reset();

		return current_result;
	} catch (const YAMLException &e) {
		return YAMLResult::error(e.what(), e.get_line(), e.get_column());
	} catch (const std::exception &e) {
		return YAMLResult::error(e.what());
	} catch (...) {
		return YAMLResult::error("Unexpected error during parsing");
	}
}

Variant YAML::Parser::process_node(const ryml::ConstNodeRef &node) const {
	// First check for tagged values
	auto tagged = try_parse_tagged_value(node);
	if (tagged) {
		return *tagged;
	}

	return process_common(node);
}

Variant YAML::Parser::process_common(const ryml::ConstNodeRef &node) const {
	if (node.is_keyval()) {
		return process_value(node);
	} else if (node.is_map()) {
		return process_map(node);
	} else if (node.is_seq()) {
		return process_sequence(node);
	} else if (node.has_key()) {
		String key = extract_key(node);
		if (!key.is_empty()) {
			return key;
		}
		return Variant();
	} else if (node.has_val()) {
		return process_value(node);
	}

	return Variant();
}

Variant YAML::Parser::process_map(const ryml::ConstNodeRef &node) const {
	Dictionary dict;

	if (detect_style) {
		Ref<YAMLStyle> map_style = context->current_style();
		map_style->set_container_form(YAMLStyle::FORM_DICTIONARY);
		if (node.is_flow()) {
			map_style->set_flow_style(YAMLStyle::FLOW_SINGLE);
		}
	}

	for (const auto &child : node.children()) {
		std::optional<String> key_result = extract_key(child);
		if (!key_result.has_value()) {
			return Variant();
		}

		String &key_ref = key_result.value();

		if (detect_style) {
			context->push_style(key_ref);
		}

		Variant value = process_node(child);

		if (detect_style) {
			context->pop_style();
		}

		if (value.get_type() == Variant::NIL && current_result->has_error()) {
			return Variant();
		}

		dict[key_ref] = value;
	}

	return dict;
}

Variant YAML::Parser::process_sequence(const ryml::ConstNodeRef &node) const {
	Array arr;

	if (detect_style) {
		Ref<YAMLStyle> seq_style = context->current_style();
		seq_style->set_container_form(YAMLStyle::FORM_ARRAY);
		if (node.is_flow()) {
			seq_style->set_flow_style(YAMLStyle::FLOW_SINGLE);
		}
	}

	size_t idx = 0;
	for (const auto &child : node.children()) {
		if (detect_style) {
			Ref<YAMLStyle> current_style = context->current_style();
			Ref<YAMLStyle> child_style = context->push_style(String::num_uint64(idx));

			// First element style is used for template
			if (idx == 0) {
				current_style->set_child("_template", child_style);
			}
		}

		Variant item = process_node(child);

		if (detect_style) {
			context->pop_style();
		}

		if (item.get_type() == Variant::NIL && current_result->has_error()) {
			return Variant();
		}

		arr.push_back(item);
		idx++;
	}

	// Remove any child styles that are identical to the template style
	if (detect_style) {
		context->current_style()->simplify();
	}

	return arr;
}

String YAML::Parser::extract_key(const ryml::ConstNodeRef &node) const {
	if (node.has_key()) {
		return from_ryml_str(node.key());
	}
	return "";
}

Variant YAML::Parser::process_value(const ryml::ConstNodeRef &node) const {
	if (!node.has_val() || node.val().empty() || node.val_is_null()) {
		return Variant();
	}

	ryml::csubstr val = node.val();
	String str_val = from_ryml_str(val);

	if (auto special_val = try_parse_special_value(str_val)) {
		return *special_val;
	}

	if (auto num_val = try_parse_numeric_value(str_val, val)) {
		return *num_val;
	}

	if (detect_style) {
		YAMLStyle::detect_string_style(node, context->current_style());
	}

	return str_val;
}

std::optional<Variant> YAML::Parser::try_parse_special_value(const String &str_val) const {
	if (str_val == "true") {
		return true;
	}

	if (str_val == "false") {
		return false;
	}

	if (str_val == "null" || str_val == "~") {
		return Variant();
	}

	if (str_val == ".inf" || str_val == "+.inf") {
		return Math_INF;
	}

	if (str_val == "-.inf") {
		return -Math_INF;
	}

	if (str_val == ".nan") {
		return Math_NAN;
	}

	return std::nullopt;
}

std::optional<Variant> YAML::Parser::try_parse_numeric_value(const String &str_val, const ryml::csubstr &val) const {
	if (str_val.begins_with("0x") || str_val.begins_with("0X") || // Hexadecimal
			str_val.begins_with("0b") || str_val.begins_with("0B") || // Binary
			str_val.begins_with("0o") || str_val.begins_with("0O") || // Octal
			(str_val.length() > 1 && str_val[0] == '0' && str_val[1] >= '0' && str_val[1] <= '7')) // Octal
	{
		try {
			YAMLStyle::IntegerFormat int_format;
			auto int_val = string_to_int<int64_t>(val, detect_style ? &int_format : nullptr);

			if (detect_style) {
				context->current_style()->set_integer_format(int_format);
			}

			return int_val;
		} catch (const std::exception &e) {
			return std::nullopt;
		}
	}

	if (str_val.contains(".")) {
		try {
			YAMLStyle::FloatFormat float_format = YAMLStyle::FLOAT_ANY;
			auto float_val = string_to_float<double>(val, detect_style ? &float_format : nullptr);

			if (detect_style) {
				context->current_style()->set_float_format(float_format);
			}

			return float_val;
		} catch (const std::exception &e) {
			return std::nullopt;
		}
	}

	if (str_val.is_valid_int()) {
		try {
			YAMLStyle::IntegerFormat int_format;
			auto int_val = string_to_int<int64_t>(val, detect_style ? &int_format : nullptr);

			if (detect_style) {
				context->current_style()->set_integer_format(int_format);
			}

			return int_val;
		} catch (const std::exception &e) {
			return std::nullopt;
		}
	}

	return std::nullopt;
}

std::optional<Variant> YAML::Parser::try_parse_tagged_value(const ryml::ConstNodeRef &node) const {
	String tag = extract_tag(node);
	if (tag.is_empty()) {
		// Return null to continue with normal processing in process_node
		return std::nullopt;
	}

	// Read !!binary as a PackedByteArray
	if (tag == "!binary") {
		tag = "PackedByteArray";
	}

	VariantConverter *converter = get_converter_for_tag(tag);
	if (converter) {
		return converter->decode(node, context.get());
	}

	if (YAMLClassRegistry::has_class(tag)) {
		YAMLClassRegistry::ClassInfo class_info = YAMLClassRegistry::get_class_info(tag);

		if (class_info.script_class.is_valid()) {
			Variant data = process_common(node);
			if (current_result->has_error()) {
				return Variant();
			}

			Variant result = class_info.script_class->call(class_info.deserialize_method, data);

			// Is it a YAMLResult?
			if (result.get_type() == Variant::OBJECT && Object::cast_to<YAMLResult>(result.operator Object *())) {
				Ref<YAMLResult> yaml_result = result;

				if (yaml_result->has_error()) {
					throw YAMLException(yaml_result->get_error_message(), ryml_parser->location(node));
				}

				return yaml_result->get_data();
			}

			return result;
		}
	}

	if (ClassDB::class_exists(tag)) {
		return parse_object_or_resource(node, tag);
	}

	// Store the tag in custom settings
	if (detect_style) {
		Ref<YAMLStyle> current_style = context->current_style();
		Dictionary custom_settings = current_style->get_custom_settings();
		custom_settings["tag"] = tag;
		current_style->set_custom_settings(custom_settings);
	}

	// Return null to continue with normal processing in process_node
	return std::nullopt;
}

Variant YAML::Parser::parse_object_or_resource(const ryml::ConstNodeRef &node, const String &class_name) const {
	// If the node contains just a string value, it might be a resource path
	if (node.has_val() && !node.is_map() && !node.is_seq()) {
		String path = from_ryml_str(node.val());

		if (path.begins_with("res://") || path.begins_with("user://")) {
			return load_resource(path, node);
		}

		throw YAMLException(vformat("Invalid resource path '%s' for class %s", path, class_name), ryml_parser->location(node));
	}

	// Otherwise, treat it as an inline object/resource definition
	if (!node.is_map()) {
		throw YAMLException(vformat("Invalid node format for class %s - expected dictionary", class_name), ryml_parser->location(node));
	}

	// Instantiate the object
	Object *obj = ClassDB::instantiate(class_name);
	if (!obj) {
		throw YAMLException(vformat("Failed to instantiate class: %s", class_name), ryml_parser->location(node));
	}

	// Process the object's properties
	populate_object_properties(obj, node);

	// Handle Resources vs regular Objects
	if (Object::cast_to<Resource>(obj)) {
		Ref<Resource> ref(Object::cast_to<Resource>(obj));
		return ref;
	}

	// For regular Objects, return the raw pointer
	// The owner of the returned Object is responsible for cleanup
	return obj;
}

Variant YAML::Parser::load_resource(const String &path, const ryml::ConstNodeRef &node) const {
	ResourceLoader *loader = ResourceLoader::get_singleton();
	if (!loader) {
		throw YAMLException("ResourceLoader singleton not available");
	}

	if (!security_view.is_path_allowed(path)) {
		throw YAMLException(vformat("Resource path not allowed: %s", path), ryml_parser->location(node));
	}

	if (!loader->exists(path)) {
		throw YAMLException(vformat("Resource file not found: %s", path), ryml_parser->location(node));
	}

	Ref<Resource> resource = loader->load(path);
	if (!resource.is_valid()) {
		throw YAMLException(vformat("Failed to load resource from path: %s", path), ryml_parser->location(node));
	}

	String class_name = resource->get_class();

	Ref<Script> script = resource->get_script();
	if (script.is_valid() && !script->get_global_name().is_empty()) {
		class_name = script->get_global_name();
	}

	if (!security_view.is_resource_allowed(path, class_name)) {
		throw YAMLException(vformat("Resource type %s not allowed from path %s", class_name, path), ryml_parser->location(node));
	}

	return resource;
}

void YAML::Parser::populate_object_properties(Object *obj, const ryml::ConstNodeRef &node) const {
	for (const auto &child : node.children()) {
		String key = from_ryml_str(child.key());

		if (detect_style) {
			context->push_style(key);
		}

		Variant value = process_node(child);

		if (detect_style) {
			context->pop_style();
		}

		if (value.get_type() == Variant::NIL) {
			continue;
		}

		obj->set(key, value);
	}
}

String YAML::Parser::extract_tag(const ryml::ConstNodeRef &node) const {
	if (!node.has_val_tag()) {
		return String();
	}

	const auto &tag = node.val_tag();
	if (tag.begins_with('!')) {
		return String::utf8(tag.sub(1).str, tag.len - 1);
	}

	return String::utf8(tag.str, tag.len);
}

VariantConverter *YAML::Parser::get_converter_for_type(Variant::Type type) const {
	auto it = type_converters.find(type);
	return it != type_converters.end() ? it->second.get() : nullptr;
}

VariantConverter *YAML::Parser::get_converter_for_tag(const String &tag) const {
	auto it = tag_converters.find(tag);
	return it != tag_converters.end() ? it->second : nullptr;
}

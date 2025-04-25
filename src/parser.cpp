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

	factory.set_parser(ryml_parser.get());
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
		throw YAMLException(from_ryml_str(error_msg));
	}

	parser->current_result = YAMLResult::error(
			from_ryml_str(error_msg),
			loc.line,
			loc.col);

	// Error handler MUST throw!
	throw YAMLException(parser->current_result->get_error_message());
}

Ref<YAMLResult> YAML::Parser::parse(const String &input, const bool p_detect_style, const YAMLSecurity::View &p_security_view) {
	try {
		detect_style = p_detect_style;
		style = detect_style ? YAML::create_style() : nullptr;
		current_result = YAMLResult::success(Variant());
		security_view = p_security_view;

		tree.clear();
		current_path.clear();

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

		if (detect_style) {
			detect_node_style(tree.rootref());
		}

		if (!current_result->has_error()) {
			if (tree.rootref().is_stream() && tree.rootref().num_children() > 1) {
				Array documents;
				for (const auto &child : tree.rootref().children()) {
					documents.push_back(process_node(child));
				}

				current_result = YAMLResult::success(documents, style);
			} else {
				Variant parsed_data = process_node(tree.rootref());
				current_result = YAMLResult::success(parsed_data, style);
			}
		}

		return current_result;
	} catch (const YAMLException &e) {
		return YAMLResult::error(e.what(), e.get_line(), e.get_column());
	} catch (const std::exception &e) {
		return YAMLResult::error(e.what());
	} catch (...) {
		return YAMLResult::error("Unknown error occurred during parsing");
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
		return process_key(node);
	} else if (node.has_val()) {
		return process_value(node);
	}

	return Variant();
}

Variant YAML::Parser::process_map(const ryml::ConstNodeRef &node) const {
	Dictionary dict;

	for (const auto &child : node.children()) {
		Variant key = process_key(child);
		if (key.get_type() == Variant::NIL && current_result->has_error()) {
			return Variant();
		}

		Variant value = process_node(child);
		if (value.get_type() == Variant::NIL && current_result->has_error()) {
			return Variant();
		}

		dict[key] = value;
	}

	return dict;
}

Variant YAML::Parser::process_sequence(const ryml::ConstNodeRef &node) const {
	Array arr;

	for (const auto &child : node.children()) {
		Variant item = process_node(child);
		if (item.get_type() == Variant::NIL && current_result->has_error()) {
			return Variant();
		}
		arr.push_back(item);
	}

	return arr;
}

Variant YAML::Parser::process_key(const ryml::ConstNodeRef &node) const {
	if (!node.has_key()) {
		return Variant();
	}
	return from_ryml_str(node.key());
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
	if (str_val.begins_with("0b") || str_val.begins_with("0B") || // Binary
			str_val.begins_with("0o") || str_val.begins_with("0O") || // Octal
			str_val.begins_with("0x") || str_val.begins_with("0X") || // Hexadecimal
			(str_val.length() > 1 && str_val[0] == '0' && str_val[1] >= '0' && str_val[1] <= '7')) // Octal
	{
		try {
			return string_to_int<int64_t>(val);
		} catch (const std::exception &e) {
			return std::nullopt;
		}
	}

	if (str_val.is_valid_int()) {
		try {
			return string_to_int<int64_t>(val);
		} catch (const std::exception &e) {
			return std::nullopt;
		}
	}

	if (str_val.is_valid_float()) {
		try {
			return string_to_float<double>(val);
		} catch (const std::exception &e) {
			return std::nullopt;
		}
	}

	return std::nullopt;
}

std::optional<Variant> YAML::Parser::try_parse_tagged_value(const ryml::ConstNodeRef &node) const {
	String tag = extract_tag(node);
	if (tag.is_empty()) {
		return std::nullopt;
	}

	// Detect PackedByteArray encoding
	if (detect_style && style.is_valid() && (tag == "PackedByteArray" || tag == "!binary")) {
		String value = from_ryml_str(node.val());
		bool is_hex = true;

		// Simple hex detection
		for (int i = 0; i < value.length(); i++) {
			char32_t c = value[i];
			if (!is_whitespace(c) && !is_hex_digit(c)) {
				is_hex = false;
				break;
			}
		}

		style->set_binary_encoding(is_hex ? YAMLStyle::BIN_HEX : YAMLStyle::BIN_BASE64);
	}

	// Read !!binary as a PackedByteArray
	if (tag == "!binary") {
		VariantConverter *converter = get_converter_for_type(Variant::PACKED_BYTE_ARRAY);
		if (converter) {
			return converter->decode(node);
		}
	}

	VariantConverter *converter = get_converter_for_tag(tag);
	if (converter) {
		return converter->decode(node);
	}

	if (YAMLClassRegistry::has_class(tag)) {
		YAMLClassRegistry::ClassInfo class_info = YAMLClassRegistry::get_class_info(tag);

		if (class_info.script_class.is_valid()) {
			Variant data = process_common(node);
			Variant result = class_info.script_class->call(class_info.deserialize_method, data);

			// Is it a YAMLResult?
			if (result.get_type() == Variant::OBJECT && Object::cast_to<YAMLResult>(result.operator Object *())) {
				Ref<YAMLResult> yaml_result = result;

				if (yaml_result->has_error()) {
					throw YAMLException(yaml_result->get_error_message());
				}

				return yaml_result->get_data();
			}

			return result;
		}
	}

	if (ClassDB::class_exists(tag)) {
		return parse_object_or_resource(node, tag);
	}

	// For unknown tags, store the tag in style if available
	if (detect_style && style.is_valid()) {
		// Get the current path's style
		Ref<YAMLStyle> current_style = style;
		for (const auto &path_element : current_path) {
			current_style = current_style->get_child(path_element);
			if (!current_style.is_valid()) {
				break;
			}
		}

		if (current_style.is_valid()) {
			// Store the tag in custom settings
			Dictionary custom_settings = current_style->get_custom_settings();
			custom_settings["tag"] = tag;
			current_style->set_custom_settings(custom_settings);
		}
	}

	// Return null to continue with normal processing in process_node
	return std::nullopt;
}

Variant YAML::Parser::parse_object_or_resource(const ryml::ConstNodeRef &node, const String &class_name) const {
	// If the node contains just a string value, it might be a resource path
	if (node.has_val() && !node.is_map() && !node.is_seq()) {
		String path = from_ryml_str(node.val());
		if (path.begins_with("res://") || path.begins_with("user://")) {
			return load_resource(path);
		}

		throw YAMLException(vformat("Invalid resource path for class %s", class_name));
	}

	// Otherwise, treat it as an inline object/resource definition
	if (!node.is_map()) {
		throw YAMLException(vformat("Invalid node format for class %s - expected map", class_name));
	}

	// Instantiate the object
	Object *obj = ClassDB::instantiate(class_name);
	if (!obj) {
		throw YAMLException(vformat("Failed to instantiate class: %s", class_name));
	}

	// Process the object's properties
	populate_object_properties(obj, node);

	// Handle Resources vs regular Objects
	if (Object::cast_to<Resource>(obj)) {
		// For Resources, return a Ref<Resource>
		Ref<Resource> ref(Object::cast_to<Resource>(obj));
		return ref;
	} else {
		// For regular Objects, return the raw pointer
		// The owner of the returned Variant is responsible for cleanup
		return obj;
	}
}

Variant YAML::Parser::load_resource(const String &path) const {
	ResourceLoader *loader = ResourceLoader::get_singleton();
	if (!loader) {
		throw YAMLException("ResourceLoader singleton not available");
	}

	if (!security_view.is_path_allowed(path)) {
		throw YAMLException(vformat("Resource path not allowed: %s", path));
	}

	if (!loader->exists(path)) {
		throw YAMLException(vformat("Resource does not exist at path: %s", path));
	}

	Ref<Resource> resource = loader->load(path);
	if (!resource.is_valid()) {
		throw YAMLException(vformat("Failed to load resource from path: %s", path));
	}

	String class_name = resource->get_class();

	Ref<Script> script = resource->get_script();
	if (script.is_valid() && !script->get_global_name().is_empty()) {
		class_name = script->get_global_name();
	}

	if (!security_view.is_resource_allowed(path, class_name)) {
		throw YAMLException(vformat("Resource type %s not allowed from path %s", class_name, path));
	}

	return resource;
}

void YAML::Parser::populate_object_properties(Object *obj, const ryml::ConstNodeRef &node) const {
	for (const auto &child : node.children()) {
		String key = from_ryml_str(child.key());
		Variant value = process_node(child);

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

void YAML::Parser::detect_node_style(const ryml::ConstNodeRef &node) {
	if (!detect_style || !style.is_valid()) {
		return;
	}

	// Get or create style for current path
	Ref<YAMLStyle> current_style;

	if (current_path.is_empty()) {
		current_style = style; // Use root style
	} else {
		// Navigate to current path
		current_style = style;
		for (const auto &path_element : current_path) {
			Ref<YAMLStyle> child = current_style->get_child(path_element);
			if (!child.is_valid()) {
				child.instantiate();
				current_style->set_child(path_element, child);
			}

			current_style = child;
		}
	}

	detect_node_style_internal(node, current_style);
}

void YAML::Parser::detect_node_style(const ryml::ConstNodeRef &node, const Ref<YAMLStyle> &target_style) {
	if (!detect_style || !target_style.is_valid()) {
		return;
	}

	detect_node_style_internal(node, target_style);
}

void YAML::Parser::detect_node_style_internal(const ryml::ConstNodeRef &node, const Ref<YAMLStyle> &current_style) {
	// Detect styles for this node
	detect_scalar_style(node, current_style);
	detect_container_form(node, current_style);

	// Store custom tag if present
	String tag = extract_tag(node);
	if (!tag.is_empty()) {
		Dictionary custom_settings = current_style->get_custom_settings();
		custom_settings["tag"] = tag;
		current_style->set_custom_settings(custom_settings);
	}

	// For map nodes, process children with updated path
	if (node.is_map()) {
		for (const auto &child : node.children()) {
			String key = String::utf8(child.key().str, child.key().len);
			auto new_path = current_path;
			new_path.append(key);
			current_path = new_path;
			detect_node_style(child); // Use original method for path-based detection
			current_path.remove_at(current_path.size() - 1);
		}
	}

	// For sequence nodes, use indices as path elements
	else if (node.is_seq()) {
		// First detect template style if needed
		detect_array_template_style(node, current_path.is_empty() ? String("_template") : current_path[current_path.size() - 1], current_style);

		// Then process each element
		int index = 0;
		for (const auto &child : node.children()) {
			auto new_path = current_path;
			new_path.push_back(String::num_int64(index++));
			current_path = new_path;
			detect_node_style(child); // Use original method for path-based detection
			current_path.remove_at(current_path.size() - 1);
		}
	}
}

void YAML::Parser::detect_scalar_style(const ryml::ConstNodeRef &node, const Ref<YAMLStyle> &style) {
	if (!node.has_val()) {
		return;
	}

	if (node.is_val_literal()) {
		style->set_string_style(YAMLStyle::STRING_LITERAL);
		return;
	} else if (node.is_val_folded()) {
		style->set_string_style(YAMLStyle::STRING_FOLDED);
		return;
	} else if (node.is_val_quoted()) {
		style->set_string_style(node.is_val_squo() ? YAMLStyle::STRING_QUOTE_SINGLE : YAMLStyle::STRING_QUOTE_DOUBLE);
		return;
	}

	String value = from_ryml_str(node.val());

	if (value.is_valid_float()) {
		if (value.find("e") != -1 || value.find("E") != -1) {
			style->set_float_format(YAMLStyle::FLOAT_SCIENTIFIC);
		} else {
			style->set_float_format(YAMLStyle::FLOAT_DECIMAL);
		}
		return;
	}

	if (value.is_valid_int()) {
		if (value.begins_with("0x") || value.begins_with("0X")) {
			style->set_integer_format(YAMLStyle::INT_HEX);
		} else if (value.begins_with("0o") || value.begins_with("0O")) {
			style->set_integer_format(YAMLStyle::INT_OCTAL);
		} else if (value.begins_with("0b") || value.begins_with("0B")) {
			style->set_integer_format(YAMLStyle::INT_BINARY);
		} else if (value.find("e") != -1 || value.find("E") != -1) {
			style->set_integer_format(YAMLStyle::INT_SCIENTIFIC);
		} else {
			style->set_integer_format(YAMLStyle::INT_DECIMAL);
		}
		return;
	}
}

void YAML::Parser::detect_container_form(const ryml::ConstNodeRef &node, const Ref<YAMLStyle> &style) {
	if (node.is_seq()) {
		style->set_container_form(YAMLStyle::FORM_SEQ);
	} else if (node.is_map()) {
		style->set_container_form(YAMLStyle::FORM_MAP);
	} else {
		style->set_container_form(YAMLStyle::FORM_ANY);
	}

	if (node.is_flow()) {
		style->set_flow_style(YAMLStyle::FLOW_SINGLE);
	} else {
		style->set_flow_style(YAMLStyle::FLOW_NONE);
	}
}

void YAML::Parser::detect_array_template_style(const ryml::ConstNodeRef &node, const String &key, Ref<YAMLStyle> current_style) {
	if (!detect_style || !current_style.is_valid() || !node.is_seq() || node.num_children() == 0) {
		return;
	}

	Ref<YAMLStyle> template_style;
	template_style.instantiate();

	// Save current path
	auto saved_path = current_path;
	current_path.clear();

	// Detect style for the first element
	detect_node_style(node[0], template_style);
	current_style->set_child("_template", template_style);

	// Restore path
	current_path = saved_path;
}

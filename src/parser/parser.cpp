#include "parser.hpp"
#include "../class_registry.hpp"
#include "../util/util_numeric.hpp"
#include "../yaml.hpp"
#include "security.hpp"
#include <schema_registry.hpp>

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

YAML::Parser::Parser(std::unordered_set<String, StringHasher, StringEqual> *shared_paths) {
	callbacks.m_error = error_callback;
	callbacks.m_user_data = this;
	evt_handler = std::make_unique<ryml::EventHandlerTree>(callbacks);
	ryml_parser = std::make_unique<ryml::Parser>(evt_handler.get(), ryml::ParserOptions().locations(true));

	if (shared_paths) {
		loading_yaml_paths = shared_paths;
		owns_yaml_paths = false;
	} else {
		loading_yaml_paths = new std::unordered_set<String, StringHasher, StringEqual>();
		owns_yaml_paths = true;
	}

	init_converters();
}

YAML::Parser::~Parser() {
	if (owns_yaml_paths) {
		delete loading_yaml_paths;
	}
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

	parser->current_result = YAMLResult::error(from_ryml_str(error_msg), loc.line, loc.col);

	// Error handler MUST throw!
	throw YAMLException(parser->current_result->get_error_message(), loc);
}

Ref<YAMLResult> YAML::Parser::parse(
		const String &input, const YAMLSecurity::View &p_security_view, const bool p_detect_style) {
	try {
		current_result = YAMLResult::success(Variant());
		security_view = p_security_view;
		style = YAML::create_style();
		detect_style = p_detect_style;

		ryml::parse_in_arena(ryml_parser.get(), input.utf8().get_data(), &tree);

		if (tree.empty()) {
			return YAMLResult::error("Empty YAML document");
		}

		// Resolve anchors and aliases
		tree.resolve();

		// Disable style detection if multiple documents
		if (tree.rootref().is_stream() && tree.rootref().num_children() > 1) {
			detect_style = false;
		}

		context = std::make_unique<YAMLParserContext>(ryml_parser.get(), detect_style ? style : nullptr);

		if (!current_result->has_error()) {
			if (tree.rootref().is_stream() && tree.rootref().num_children() > 1) {
				// Handle multi-document parsing
				Array documents;
				for (const auto &child : tree.rootref().children()) {
					documents.push_back(process_node(child));
				}
				current_result = YAMLResult::multi_document_success(documents);
			} else {
				// Single document result
				current_result = YAMLResult::success(process_node(tree.rootref()), style);
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

Ref<YAMLResult> YAML::Parser::parse_and_validate(const String &input,
		const Variant &schema_param, // null, String (ID), or Ref<Schema>
		const YAMLSecurity::View &security_view, const bool detect_style) {
	try {
		current_result = YAMLResult::success(Variant());
		this->security_view = security_view;
		this->style = YAML::create_style();
		this->detect_style = detect_style;

		// Parse YAML text
		ryml::parse_in_arena(ryml_parser.get(), input.utf8().get_data(), &tree);

		if (tree.empty()) {
			return YAMLResult::error("Empty YAML document");
		}

		tree.resolve();

		// Resolve explicit schema
		Ref<Schema> schema = resolve_schema_parameter(schema_param);

		// Create context with validation enabled
		context = std::make_unique<YAMLParserContext>(ryml_parser.get(), detect_style ? style : nullptr, schema);

		// Process YAML tree with validation
		Variant data = process_node(tree.rootref());

		if (current_result->has_error()) {
			return current_result;
		}

		// Build validation result
		Ref<SchemaValidationResult> validation_result;
		if (context->has_validation_errors()) {
			validation_result = SchemaValidationResult::success();
			for (const YAMLValidationError &error : context->get_validation_errors()) {
				// Add errors to result (we'd need to add this method to SchemaValidationResult)
				validation_result->add_error(error);
			}
		}

		context.reset();

		return YAMLResult::success(data, style, validation_result);

	} catch (const YAMLException &e) {
		return YAMLResult::error(e.what(), e.get_line(), e.get_column());
	} catch (const std::exception &e) {
		return YAMLResult::error(e.what());
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
	Variant result;

	// Parse structure first
	if (node.is_keyval()) {
		result = process_value(node);
	} else if (node.is_map()) {
		result = process_map(node);
	} else if (node.is_seq()) {
		result = process_sequence(node);
	} else if (node.has_key()) {
		String key = extract_key(node);
		if (!key.is_empty()) {
			result = key;
		} else {
			result = Variant();
		}
	} else if (node.has_val()) {
		result = process_value(node);
	} else {
		result = Variant();
	}

	if (context->is_validating()) {
		validate_current_node(result);
	}

	return result;
}

Variant YAML::Parser::process_map(const ryml::ConstNodeRef &node) const {
	Dictionary dict;

	if (context->needs_schema_discovery() && context->get_current_instance_path() == "/") {
		discover_and_load_schema(node);
	}

	if (detect_style) {
		Ref<YAMLStyle> map_style = context->current_style();
		map_style->set_container_form(YAMLStyle::FORM_DICTIONARY);
		if (node.is_flow()) {
			map_style->set_flow_style(YAMLStyle::FLOW_SINGLE);
		}
	}

	for (const auto &child : node.children()) {
		String key = extract_key(child);
		if (key.is_empty()) {
			return Variant();
		}

		// Track paths
		if (context->is_validating()) {
			context->push_instance_path_segment(key);
			context->push_schema_for_property(key);
		}

		if (detect_style) {
			context->push_style(key);
		}

		Variant value = process_node(child);

		if (detect_style) {
			context->pop_style();
		}

		if (context->is_validating()) {
			context->pop_schema();
			context->pop_instance_path_segment();
		}

		if (value.get_type() == Variant::NIL && current_result->has_error()) {
			return Variant();
		}

		dict[key] = value;
	}

	if (context->is_validating()) {
		apply_property_defaults(dict);
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
		// Track paths
		if (context->is_validating()) {
			context->push_instance_path_segment(String::num_uint64(idx));
			context->push_schema_for_array_item(idx);
		}

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

		if (context->is_validating()) {
			context->pop_schema();
			context->pop_instance_path_segment();
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
	if (!node.has_val() || node.val_is_null()) {
		return Variant();
	}

	ryml::csubstr val = node.val();
	String str_val = from_ryml_str(val);

	// Quoted and block style values are always strings
	if (node.is_val_quoted() || node.is_block()) {
		if (detect_style) {
			YAMLStyle::detect_string_style(node, context->current_style());
		}

		return str_val;
	}

	// Empty string is null in YAML
	if (str_val.length() == 0) {
		return Variant();
	}

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
		if (detect_style) {
			context->current_style()->set_string_style(YAMLStyle::STRING_PLAIN);
		}
		return true;
	}

	if (str_val == "false") {
		if (detect_style) {
			context->current_style()->set_string_style(YAMLStyle::STRING_PLAIN);
		}
		return false;
	}

	if (str_val == "null" || str_val == "~") {
		if (detect_style) {
			context->current_style()->set_string_style(YAMLStyle::STRING_PLAIN);
		}
		return Variant();
	}

	if (str_val == ".inf" || str_val == "+.inf") {
		if (detect_style) {
			context->current_style()->set_float_format(YAMLStyle::FloatFormat::FLOAT_ANY);
		}
		return Math_INF;
	}

	if (str_val == "-.inf") {
		if (detect_style) {
			context->current_style()->set_float_format(YAMLStyle::FloatFormat::FLOAT_ANY);
		}
		return -Math_INF;
	}

	if (str_val == ".nan") {
		if (detect_style) {
			context->current_style()->set_float_format(YAMLStyle::FloatFormat::FLOAT_ANY);
		}
		return Math_NAN;
	}

	return std::nullopt;
}

std::optional<Variant> YAML::Parser::try_parse_numeric_value(const String &str_val, const ryml::csubstr &val) const {
	// Helper function to validate hexadecimal string
	auto is_valid_hex = [](const String &s) -> bool {
		if (s.length() > 20) {
			return false; // 64-bit hex (16 digits) + "0x" + sign = reasonable limit
		}

		size_t start = 0;
		if (s[0] == '+' || s[0] == '-') {
			start = 1;
		}

		if (start + 2 >= s.length()) {
			return false;
		}
		if (s[start] != '0' || (s[start + 1] != 'x' && s[start + 1] != 'X')) {
			return false;
		}

		for (int i = start + 2; i < s.length(); i++) {
			char c = s[i];
			if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
				return false;
			}
		}
		return true;
	};

	// Helper function to validate binary string
	auto is_valid_binary = [](const String &s) -> bool {
		if (s.length() > 68) {
			return false; // 64-bit binary (64 digits) + "0b" + sign = reasonable limit
		}

		size_t start = 0;
		if (s[0] == '+' || s[0] == '-') {
			start = 1;
		}

		if (start + 2 >= s.length()) {
			return false;
		}
		if (s[start] != '0' || (s[start + 1] != 'b' && s[start + 1] != 'B')) {
			return false;
		}

		for (int i = start + 2; i < s.length(); i++) {
			char c = s[i];
			if (c != '0' && c != '1') {
				return false;
			}
		}
		return true;
	};

	// Helper function to validate octal string (YAML 1.2 explicit format only)
	auto is_valid_octal = [](const String &s) -> bool {
		if (s.length() > 26) {
			return false; // 64-bit octal (~22 digits) + "0o" + sign = reasonable limit
		}

		size_t start = 0;
		if (s[0] == '+' || s[0] == '-') {
			start = 1;
		}

		if (start + 2 >= s.length()) {
			return false;
		}
		if (s[start] != '0' || (s[start + 1] != 'o' && s[start + 1] != 'O')) {
			return false;
		}

		for (int i = start + 2; i < s.length(); i++) {
			char c = s[i];
			if (c < '0' || c > '7') {
				return false;
			}
		}
		return true;
	};

	// Helper function to validate decimal integer string
	auto is_valid_decimal_int = [](const String &s) -> bool {
		if (s.length() > 22) {
			return false; // 64-bit decimal (~20 digits) + sign = reasonable limit
		}

		size_t start = 0;
		if (s[0] == '+' || s[0] == '-') {
			start = 1;
			if (s.length() == 1) {
				return false; // Just a sign is not valid
			}
		}

		for (int i = start; i < s.length(); i++) {
			char c = s[i];
			if (c < '0' || c > '9') {
				return false;
			}
		}
		return true;
	};

	// Helper function to validate floating point string
	auto is_valid_float = [](const String &s) -> bool {
		if (s.length() > 50) {
			return false; // Reasonable limit for float strings
		}

		bool has_dot = false;
		bool has_e = false;
		bool has_sign_after_e = false;
		size_t start = 0;

		// Handle leading sign
		if (s[0] == '+' || s[0] == '-') {
			start = 1;
			if (s.length() == 1) {
				return false;
			}
		}

		for (int i = start; i < s.length(); i++) {
			char c = s[i];

			if (c >= '0' && c <= '9') {
				continue; // Digits are always okay
			} else if (c == '.') {
				if (has_dot || has_e) {
					return false; // Only one dot, and not after 'e'
				}
				has_dot = true;
			} else if (c == 'e' || c == 'E') {
				if (has_e) {
					return false; // Only one 'e'
				}
				if (i == start) {
					return false; // Can't start with 'e'
				}
				has_e = true;
			} else if ((c == '+' || c == '-') && has_e && !has_sign_after_e) {
				// Sign after 'e' is allowed, but only once and immediately after 'e'
				if (i == 0 || (s[i - 1] != 'e' && s[i - 1] != 'E')) {
					return false;
				}
				has_sign_after_e = true;
			} else {
				return false; // Invalid character
			}
		}

		// Must have at least one digit and a dot to be a valid float
		return has_dot;
	};

	// Check for explicit numeric formats (hex, binary, octal)
	// At minimum "0x", "0b", or "0o" + 1 digit
	if (str_val.length() >= 3 && (is_valid_hex(str_val) || is_valid_binary(str_val) || is_valid_octal(str_val))) {
		try {
			YAMLStyle::IntegerFormat int_format = YAMLStyle::INT_ANY;
			auto int_val = string_to_int<int64_t>(val, detect_style ? &int_format : nullptr);

			if (detect_style) {
				context->current_style()->set_integer_format(int_format);
			}

			return int_val;
		} catch (const std::exception &e) {
			return std::nullopt;
		}
	}

	if (str_val.contains(".") && is_valid_float(str_val)) {
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

	if (is_valid_decimal_int(str_val)) {
		try {
			YAMLStyle::IntegerFormat int_format = YAMLStyle::INT_DECIMAL;
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

	if (context->is_validating()) {
		check_yaml_tag_constraint(tag);
	}

	if (tag.is_empty()) {
		// Return null to continue with normal processing in process_node
		return std::nullopt;
	}

	// Read !!binary as a PackedByteArray
	if (tag == "!binary") {
		tag = "PackedByteArray";
	}

	YAMLVariantConverter *converter = get_converter_for_tag(tag);
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
					throw YAMLException(yaml_result->get_error_message(), node.location(*ryml_parser));
				}

				return yaml_result->get_data();
			}

			return result;
		}
	}

	if (ClassDB::class_exists(tag)) {
		return parse_object_or_resource(node, tag);
	}

	// Store custom tag
	if (detect_style && !tag.is_empty()) {
		context->current_style()->set_custom_tag(tag);
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

		throw YAMLException(
				vformat("Invalid resource path '%s' for class %s", path, class_name), node.location(*ryml_parser));
	}

	// Otherwise, treat it as an inline object/resource definition
	if (!node.is_map()) {
		throw YAMLException(vformat("Invalid node format for class %s - expected dictionary", class_name),
				node.location(*ryml_parser));
	}

	// Instantiate the object
	Object *obj = ClassDB::instantiate(class_name);
	if (!obj) {
		throw YAMLException(vformat("Failed to instantiate class: %s", class_name), node.location(*ryml_parser));
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
	if (path.ends_with(".yaml") || path.ends_with(".yml")) {
		if (!security_view.is_path_allowed(path)) {
			throw YAMLException(vformat("Resource path not allowed: %s", path), node.location(*ryml_parser));
		}

		if (loading_yaml_paths->find(path) != loading_yaml_paths->end()) {
			throw YAMLException(vformat("Cyclical YAML reference detected: %s", path), node.location(*ryml_parser));
		}

		loading_yaml_paths->insert(path);

		// Track nested resource context
		if (context->is_validating()) {
			context->push_resource_context(path);
		}

		Ref<YAMLResult> result =
				YAML::parser_load_file(path, security_view, loading_yaml_paths, context->is_validating());

		if (context->is_validating()) {
			// Collect nested validation errors with corrected paths
			if (result->has_validation_errors()) {
				Array nested_errors = result->get_validation_errors();
				String resource_prefix = vformat("!Resource(%s)", path);

				for (int i = 0; i < nested_errors.size(); i++) {
					Dictionary error_dict = nested_errors[i];

					// Build path string with resource context
					String nested_path = error_dict["path"];
					String full_path = resource_prefix + nested_path;

					// Build clean path_array (only actual property/index segments)
					Array nested_path_array = error_dict["path_array"];
					Array full_path_array;
					full_path_array.append_array(context->get_current_instance_path_array());
					full_path_array.append_array(nested_path_array);

					YAMLValidationError error(error_dict["message"], full_path, full_path_array,
							error_dict["constraint"], error_dict.get("schema_path", ""),
							error_dict.get("invalid_value", Variant()));

					context->add_validation_error(error);
				}
			}

			context->pop_resource_context();
		}

		loading_yaml_paths->erase(path);

		if (result->has_error()) {
			throw YAMLException(vformat("Failed to load YAML resource: %s - %s", path, result->get_error()),
					node.location(*ryml_parser));
		}

		return result->get_data();
	}

	ResourceLoader *loader = ResourceLoader::get_singleton();
	if (!loader) {
		throw YAMLException("ResourceLoader singleton not available");
	}

	if (!security_view.is_path_allowed(path)) {
		throw YAMLException(vformat("Resource path not allowed: %s", path), node.location(*ryml_parser));
	}

	if (!loader->exists(path)) {
		throw YAMLException(vformat("Resource file not found: %s", path), node.location(*ryml_parser));
	}

	Ref<Resource> resource = loader->load(path);
	if (!resource.is_valid()) {
		throw YAMLException(vformat("Failed to load resource from path: %s", path), node.location(*ryml_parser));
	}

	String class_name = resource->get_class();

	Ref<Script> script = resource->get_script();
	if (script.is_valid() && !script->get_global_name().is_empty()) {
		class_name = script->get_global_name();
	}

	if (!security_view.is_resource_allowed(path, class_name)) {
		throw YAMLException(
				vformat("Resource type %s not allowed from path %s", class_name, path), node.location(*ryml_parser));
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

void YAML::Parser::validate_current_node(const Variant &value) const {
	Ref<Schema> schema = context->current_schema();
	if (!schema.is_valid()) {
		return; // No schema for this node
	}

	// Perform validation
	Ref<SchemaValidationResult> validation = schema->validate(value);

	if (validation->has_errors()) {
		// Collect all errors with full path context
		String base_path = context->get_current_instance_path();
		Array resource_stack = context->get_resource_path_stack();

		for (int i = 0; i < validation->error_count(); i++) {
			Dictionary error_dict = validation->get_error(i);

			// Build full path including nested resources
			String full_path;

			// Add resource context if present
			if (resource_stack.size() > 0) {
				for (int j = 0; j < resource_stack.size(); j++) {
					full_path += vformat("!Resource(%s)", resource_stack[j]);
				}
			}

			// Add base path (avoid double slashes)
			if (!base_path.is_empty() && base_path != "/") {
				full_path += base_path;
			}

			// Add error-specific path
			String error_path = error_dict["path"];
			if (!error_path.is_empty() && error_path != "/") {
				// If full_path is empty or just "/", start fresh
				if (full_path.is_empty() || full_path == "/") {
					full_path = error_path;
				} else {
					full_path += error_path;
				}
			} else if (full_path.is_empty()) {
				full_path = "/";
			}

			YAMLValidationError error(error_dict["message"], full_path, error_dict["path_array"],
					error_dict["constraint"], error_dict.get("schema_path", ""),
					error_dict.get("invalid_value", Variant()));

			context->add_validation_error(error);
		}
	}
}

Ref<Schema> YAML::Parser::resolve_schema_parameter(const Variant &schema_param) const {
	if (schema_param.get_type() == Variant::NIL) {
		return Ref<Schema>();
	}

	// Direct Schema object
	if (schema_param.get_type() == Variant::OBJECT) {
		Ref<Schema> schema = schema_param;
		if (schema.is_valid()) {
			return schema;
		}
	}

	// Schema ID string
	if (schema_param.get_type() == Variant::STRING) {
		String schema_id = schema_param;
		if (!schema_id.is_empty() && Schema::is_schema_registered(schema_id)) {
			return SchemaRegistry::get_singleton().get_schema(schema_id);
		}
	}

	return Ref<Schema>();
}

void YAML::Parser::discover_and_load_schema(const ryml::ConstNodeRef &node) const {
	for (const auto &child : node.children()) {
		String key = extract_key(child);
		if (key == "$schema" && child.has_val()) {
			String schema_id = from_ryml_str(child.val());
			if (Schema::is_schema_registered(schema_id)) {
				Ref<Schema> schema = SchemaRegistry::get_singleton().get_schema(schema_id);
				context->set_root_schema(schema);
			}
			return;
		}
	}
}

void YAML::Parser::check_yaml_tag_constraint(const String &tag) const {
	Ref<Schema> schema = context->current_schema();
	if (!schema.is_valid()) {
		return;
	}

	Variant required_tag = schema->get_custom_metadata("x-yaml-tag");
	if (required_tag.get_type() != Variant::STRING) {
		return; // No tag constraint
	}

	String expected_tag = required_tag;

	if (!tag.is_empty() && tag != expected_tag) {
		YAMLValidationError error(vformat("Expected YAML tag '!%s' but got '!%s'", expected_tag, tag),
				context->get_current_instance_path(), context->get_current_instance_path_array(), "x-yaml-tag",
				schema->get_schema_path(),
				Variant() // No specific invalid value
		);
		context->add_validation_error(error);
	} else if (tag.is_empty() && !expected_tag.is_empty()) {
		YAMLValidationError error(vformat("Expected YAML tag '!%s' but value has no tag", expected_tag),
				context->get_current_instance_path(), Array(), "x-yaml-tag", schema->get_schema_path(), Variant());
		context->add_validation_error(error);
	}
}

void YAML::Parser::apply_property_defaults(Dictionary &dict) const {
	Ref<Schema> schema = context->current_schema();
	if (!schema.is_valid() || !schema->is_object()) {
		return;
	}

	// Iterate through all properties defined in the schema
	Array property_keys = schema->get_child_keys();
	for (int i = 0; i < property_keys.size(); i++) {
		StringName key = property_keys[i];
		String key_str = key;

		// Only consider "properties/xxx" keys
		if (!key_str.begins_with("properties/")) {
			continue;
		}

		// Extract the actual property name (remove "properties/" prefix)
		String property_name = key_str.substr(11);

		// Skip if property already exists in the dictionary
		if (dict.has(property_name)) {
			continue;
		}

		// Get the schema for this property
		Ref<Schema> prop_schema = schema->get_child(key);
		if (!prop_schema.is_valid() || !prop_schema->has_default_value()) {
			continue;
		}

		// Apply the default value
		Variant default_value = prop_schema->get_default_value();
		dict[property_name] = default_value;
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

YAMLVariantConverter *YAML::Parser::get_converter_for_type(Variant::Type type) const {
	auto it = type_converters.find(type);
	return it != type_converters.end() ? it->second.get() : nullptr;
}

YAMLVariantConverter *YAML::Parser::get_converter_for_tag(const String &tag) const {
	auto it = tag_converters.find(tag);
	return it != tag_converters.end() ? it->second : nullptr;
}

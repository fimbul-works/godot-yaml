#include "yaml.hpp"
#include "class_registry.hpp"
#include "emitter/emitter.hpp"
#include "parser/parser.hpp"
#include "parser/security.hpp"
#include "result.hpp"
#include "style/style.hpp"
#include "style/style_view.hpp"
#include "syntax_validator/syntax_validator.hpp"
#include "version.hpp"
#include <validation_result.hpp>

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <sstream>

using namespace godot;

void YAML::_bind_methods() {
	ClassDB::bind_static_method("YAML", D_METHOD("version"), &YAML::version);

	ClassDB::bind_static_method("YAML", D_METHOD("parse", "yaml_text", "security", "detect_style"), &YAML::parse, DEFVAL(nullptr), DEFVAL(false));
	ClassDB::bind_static_method("YAML", D_METHOD("parse_and_validate", "yaml_text", "schema", "security", "detect_style"), &YAML::parse_and_validate, DEFVAL(Variant()), DEFVAL(nullptr), DEFVAL(false));

	ClassDB::bind_static_method("YAML", D_METHOD("stringify", "data", "style"), &YAML::stringify, DEFVAL(Variant()));

	ClassDB::bind_static_method("YAML", D_METHOD("validate_syntax", "yaml_text"), &YAML::validate_syntax);
	ClassDB::bind_static_method("YAML", D_METHOD("validate_file_syntax", "path"), &YAML::validate_file_syntax);

	ClassDB::bind_static_method("YAML", D_METHOD("load_file", "path", "security", "detect_style"), &YAML::load_file, DEFVAL(nullptr), DEFVAL(false));
	ClassDB::bind_static_method("YAML", D_METHOD("load_file_and_validate", "path", "schema", "security", "detect_style"), &YAML::load_file_and_validate, DEFVAL(Variant()), DEFVAL(nullptr), DEFVAL(false));

	ClassDB::bind_static_method("YAML", D_METHOD("save_file", "data", "path", "style"), &YAML::save_file, DEFVAL(nullptr));

	ClassDB::bind_static_method("YAML", D_METHOD("try_parse", "yaml_text", "security"), &YAML::try_parse, DEFVAL(nullptr));
	ClassDB::bind_static_method("YAML", D_METHOD("try_parse_and_validate", "yaml_text", "schema", "security"), &YAML::try_parse_and_validate, DEFVAL(Variant()), DEFVAL(nullptr));

	ClassDB::bind_static_method("YAML", D_METHOD("try_stringify", "data", "style"), &YAML::try_stringify, DEFVAL(Variant()));

	ClassDB::bind_static_method("YAML", D_METHOD("try_load_file", "path", "security"), &YAML::try_load_file, DEFVAL(nullptr));
	ClassDB::bind_static_method("YAML", D_METHOD("try_load_file_and_validate", "path", "schema", "security"), &YAML::try_load_file_and_validate, DEFVAL(Variant()), DEFVAL(nullptr));

	ClassDB::bind_static_method("YAML", D_METHOD("try_save_file", "data", "path", "style"), &YAML::try_save_file, DEFVAL(nullptr));

	ClassDB::bind_static_method("YAML", D_METHOD("register_class", "script_class", "serialize_method", "deserialize_method"), &YAML::register_class, DEFVAL("serialize"), DEFVAL("deserialize"), DEFVAL(nullptr));
	ClassDB::bind_static_method("YAML", D_METHOD("unregister_class", "script_class"), &YAML::unregister_class);
	ClassDB::bind_static_method("YAML", D_METHOD("has_registered_class", "script_class"), &YAML::has_registered_class);

	ClassDB::bind_static_method("YAML", D_METHOD("create_style"), &YAML::create_style);

	ClassDB::bind_static_method("YAML", D_METHOD("create_security"), &YAML::create_security);
	ClassDB::bind_static_method("YAML", D_METHOD("allow_resource_path", "path_prefix", "type_names"), &YAML::allow_resource_path, DEFVAL(Array()));
	ClassDB::bind_static_method("YAML", D_METHOD("block_resource_type", "type_name"), &YAML::block_resource_type);
	ClassDB::bind_static_method("YAML", D_METHOD("reset_security"), &YAML::reset_security);

	BIND_VIRTUAL_METHOD(YAML, _to_string);
}

String YAML::version() {
#ifdef GODOT_YAML_DEBUG
	String target = "debug";
#else
	String target = "release";
#endif
	return vformat("%s (%s)", GODOT_YAML_VERSION, target);
}

Ref<YAMLResult> YAML::parse(const String &input, const Ref<YAMLSecurity> security, const bool detect_style) {
	Parser parser;
	return parser.parse(input, security.is_valid() ? security->get_view() : YAMLSecurity::get_default_view(), detect_style);
}

Ref<YAMLResult> YAML::parse_and_validate(const String &input, const Variant &schema, const Ref<YAMLSecurity> security, const bool detect_style) {
	Parser parser;
	return parser.parse_and_validate(input, schema, security.is_valid() ? security->get_view() : YAMLSecurity::get_default_view(), detect_style);
}

Ref<YAMLResult> YAML::stringify(const Variant &input, const Ref<YAMLStyle> &style) {
	Emitter emitter;
	YAMLStyle::View style_view = style.is_valid() ? YAMLStyle::View::create_view(style) : YAMLStyle::View();
	Ref<YAMLResult> result = emitter.emit(input, style_view);
	return result;
}

Ref<YAMLResult> YAML::validate_syntax(const String &input) {
	SyntaxValidator validator;
	Ref<YAMLResult> result = validator.validate(input);
	return result;
}

Ref<YAMLResult> YAML::load_file(const String &path, const Ref<YAMLSecurity> security, const bool detect_style) {
	if (!FileAccess::file_exists(path)) {
		return YAMLResult::error("File not found '" + path + "': " + UtilityFunctions::error_string(ERR_FILE_NOT_FOUND));
	}

	// Open file for reading
	Ref<FileAccess> file = FileAccess::open(path, FileAccess::READ);

	if (!file.is_valid()) {
		return YAMLResult::error("Failed to read '" + path + "': " + UtilityFunctions::error_string(ERR_FILE_BAD_PATH));
	}

	Error err = file->get_error();
	if (err != OK) {
		return YAMLResult::error("Failed to read '" + path + "': " + UtilityFunctions::error_string((int)err));
	}

	// Read the file content and check for errors
	String content = file->get_as_text();

	err = file->get_error();
	file->close();

	if (err != OK) {
		return YAMLResult::error("Failed to read '" + path + "': " + UtilityFunctions::error_string((int)err));
	}

	return parse(content, security, detect_style);
}

Ref<YAMLResult> YAML::load_file_and_validate(const String &path, const Variant &schema, const Ref<YAMLSecurity> security, const bool detect_style) {
	if (!FileAccess::file_exists(path)) {
		return YAMLResult::error("File not found '" + path + "': " + UtilityFunctions::error_string(ERR_FILE_NOT_FOUND));
	}

	// Open file for reading
	Ref<FileAccess> file = FileAccess::open(path, FileAccess::READ);

	if (!file.is_valid()) {
		return YAMLResult::error("Failed to read '" + path + "': " + UtilityFunctions::error_string(ERR_FILE_BAD_PATH));
	}

	Error err = file->get_error();
	if (err != OK) {
		return YAMLResult::error("Failed to read '" + path + "': " + UtilityFunctions::error_string((int)err));
	}

	// Read the file content and check for errors
	String content = file->get_as_text();

	err = file->get_error();
	file->close();

	if (err != OK) {
		return YAMLResult::error("Failed to read '" + path + "': " + UtilityFunctions::error_string((int)err));
	}

	return parse_and_validate(content, schema, security, detect_style);
}

Ref<YAMLResult> YAML::parser_load_file(const String &path, const YAMLSecurity::View &security_view, std::unordered_set<String, StringHasher, StringEqual> *loading_yaml_paths, const bool is_validating) {
	if (!FileAccess::file_exists(path)) {
		return YAMLResult::error("File not found '" + path + "': " + UtilityFunctions::error_string(ERR_FILE_NOT_FOUND));
	}

	// Open file for reading
	Ref<FileAccess> file = FileAccess::open(path, FileAccess::READ);

	if (!file.is_valid()) {
		return YAMLResult::error("Failed to read '" + path + "': " + UtilityFunctions::error_string(ERR_FILE_BAD_PATH));
	}

	Error err = file->get_error();
	if (err != OK) {
		return YAMLResult::error("Failed to read '" + path + "': " + UtilityFunctions::error_string((int)err));
	}

	// Read the file content and check for errors
	String content = file->get_as_text();

	err = file->get_error();
	file->close();

	if (err != OK) {
		return YAMLResult::error("Failed to read '" + path + "': " + UtilityFunctions::error_string((int)err));
	}

	Parser parser(loading_yaml_paths);
	if (is_validating) {
		return parser.parse_and_validate(content, Variant(), security_view, false);
	}
	return parser.parse(content, security_view, false);
}

Ref<YAMLResult> YAML::save_file(const Variant &data, const String &path, const Ref<YAMLStyle> &style) {
	Ref<YAMLResult> stringify_result = stringify(data, style);
	if (stringify_result->has_error()) {
		return stringify_result;
	}

	String yaml_content = stringify_result->get_data();

	// Open file for writing
	Ref<FileAccess> file = FileAccess::open(path, FileAccess::WRITE);

	if (!file.is_valid()) {
		return YAMLResult::error("Failed to write '" + path + "': " + UtilityFunctions::error_string(ERR_FILE_BAD_PATH));
	}

	Error err = file->get_error();
	if (err != OK) {
		return YAMLResult::error("Failed to write '" + path + "': " + UtilityFunctions::error_string((int)err));
	}

	// Write the file content and check for errors
	file->store_string(yaml_content);
	err = file->get_error();
	file->close();

	if (err != OK) {
		return YAMLResult::error("Failed to write '" + path + "': " + UtilityFunctions::error_string((int)err));
	}

	// Return success with the YAML content
	return YAMLResult::success(yaml_content);
}

Ref<YAMLResult> YAML::validate_file_syntax(const String &path) {
	if (!FileAccess::file_exists(path)) {
		return YAMLResult::error("File not found '" + path + "': " + UtilityFunctions::error_string(ERR_FILE_NOT_FOUND));
	}

	// Open file for reading
	Ref<FileAccess> file = FileAccess::open(path, FileAccess::READ);

	if (!file.is_valid()) {
		return YAMLResult::error("Failed to validate syntax of '" + path + "': " + UtilityFunctions::error_string(ERR_FILE_BAD_PATH));
	}

	Error err = file->get_error();
	if (err != OK) {
		return YAMLResult::error("Failed to validate syntax of '" + path + "': " + UtilityFunctions::error_string((int)err));
	}

	// Read the file content and check for errors
	String content = file->get_as_text();
	err = file->get_error();
	file->close();

	if (err != OK) {
		return YAMLResult::error("Failed to validate syntax of '" + path + "': " + UtilityFunctions::error_string((int)err));
	}

	return validate_syntax(content);
}

Variant YAML::try_parse(const String &input, const Ref<YAMLSecurity> security) {
	Ref<YAMLResult> result = parse(input, security);
	if (result->has_error()) {
		UtilityFunctions::push_error(result->get_error());
		return Variant();
	}
	return result->get_data();
}

Variant YAML::try_parse_and_validate(const String &input, const Variant &schema, const Ref<YAMLSecurity> security) {
	Ref<YAMLResult> result = parse_and_validate(input, schema, security);
	if (result->has_error()) {
		UtilityFunctions::push_error(result->get_error());
		return Variant();
	}
	return result->get_data();
}

String YAML::try_stringify(const Variant &input, const Ref<YAMLStyle> &style) {
	Ref<YAMLResult> result = stringify(input, style);
	if (result->has_error()) {
		UtilityFunctions::push_error(result->get_error());
		return "";
	}
	return result->get_data();
}

Variant YAML::try_load_file(const String &path, const Ref<YAMLSecurity> security) {
	Ref<YAMLResult> result = load_file(path, security);
	if (result->has_error()) {
		UtilityFunctions::push_error(result->get_error());
		return Variant();
	}
	return result->get_data();
}

Variant YAML::try_load_file_and_validate(const String &path, const Variant &schema, const Ref<YAMLSecurity> security) {
	Ref<YAMLResult> result = load_file_and_validate(path, schema, security);
	if (result->has_error()) {
		UtilityFunctions::push_error(result->get_error());
		return Variant();
	}
	return result->get_data();
}

bool YAML::try_save_file(const Variant &data, const String &path, const Ref<YAMLStyle> &style) {
	Ref<YAMLResult> result = save_file(data, path, style);
	if (result->has_error()) {
		UtilityFunctions::push_error(result->get_error());
		return false;
	}
	return true;
}

Ref<YAMLStyle> YAML::create_style() {
	return Ref<YAMLStyle>(memnew(YAMLStyle()));
}

void YAML::register_class(Ref<Script> p_class, const Variant &p_serialize, const Variant &p_deserialize, const Variant &p_tag) {
	YAMLClassRegistry::register_class(p_class, p_serialize, p_deserialize, p_tag);
}

void YAML::unregister_class(Ref<Script> p_class) {
	YAMLClassRegistry::unregister_class(p_class);
}

bool YAML::has_registered_class(const String &class_name) {
	return YAMLClassRegistry::has_class(class_name);
}

Ref<YAMLSecurity> YAML::create_security() {
	return Ref<YAMLSecurity>(memnew(YAMLSecurity()));
}

void YAML::allow_resource_path(const String &path_prefix, const Array &type_names) {
	YAMLSecurity::get_default_instance()->allow_path(path_prefix, type_names);
}

void YAML::block_resource_type(const StringName &type_name) {
	YAMLSecurity::get_default_instance()->block_type(type_name);
}

void YAML::reset_security() {
	YAMLSecurity::get_default_instance()->reset();
}

String YAML::_to_string() const {
	return vformat("YAML(%s)", GODOT_YAML_VERSION);
}

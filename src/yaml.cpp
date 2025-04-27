#include "yaml.h"
#include "class_registry.h"
#include "emitter.h"
#include "parser.h"
#include "result.h"
#include "security.h"
#include "style_view.h"
#include "validator.h"
#include "version.h"

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <sstream>

using namespace godot;

void YAML::_bind_methods() {
	ClassDB::bind_static_method("YAML", D_METHOD("version"), &YAML::version);

	ClassDB::bind_static_method("YAML", D_METHOD("validate", "input"), &YAML::validate);
	ClassDB::bind_static_method("YAML", D_METHOD("parse", "input", "security", "detect_style"), &YAML::parse, DEFVAL(nullptr), DEFVAL(false));
	ClassDB::bind_static_method("YAML", D_METHOD("stringify", "input", "style"), &YAML::stringify, DEFVAL(Variant()));

	ClassDB::bind_static_method("YAML", D_METHOD("load_file", "path", "security", "detect_style"), &YAML::load_file, DEFVAL(nullptr), DEFVAL(false));
	ClassDB::bind_static_method("YAML", D_METHOD("save_file", "data", "path", "style"), &YAML::save_file, DEFVAL(nullptr));

	ClassDB::bind_static_method("YAML", D_METHOD("register_class", "script_class", "serialize", "deserialize"), &YAML::register_class, DEFVAL("serialize"), DEFVAL("deserialize"));
	ClassDB::bind_static_method("YAML", D_METHOD("has_registered_class", "tag_name"), &YAML::has_registered_class);

	ClassDB::bind_static_method("YAML", D_METHOD("create_style"), &YAML::create_style);

	ClassDB::bind_static_method("YAML", D_METHOD("create_security"), &YAML::create_security);
	ClassDB::bind_static_method("YAML", D_METHOD("allow_resource_path", "path_prefix", "type_names"), &YAML::allow_resource_path, DEFVAL(Array()));
	ClassDB::bind_static_method("YAML", D_METHOD("block_resource_type", "type_name"), &YAML::block_resource_type);
	ClassDB::bind_static_method("YAML", D_METHOD("reset_security"), &YAML::reset_security);
}

String YAML::version() {
#ifdef GODOT_YAML_DEBUG
	String target = "debug";
#else
	String target = "release";
#endif
	return String("Godot YAML " + String(GODOT_YAML_VERSION) + " (" + target + ")");
}

Ref<YAMLResult> YAML::validate(const String &input) {
	Validator validator;
	Ref<YAMLResult> result = validator.validate(input);
	return result;
}

Ref<YAMLResult> YAML::parse(const String &input, const Ref<YAMLSecurity> security, const bool detect_style) {
	Parser parser;
	return parser.parse(input, security.is_valid() ? security->get_view() : YAMLSecurity::get_default_view(), detect_style);
}

Ref<YAMLResult> YAML::stringify(const Variant &input, const Ref<YAMLStyle> &style) {
	Emitter emitter;
	YAMLStyle::View style_view = style.is_valid() ? YAMLStyle::View::create_view(style) : YAMLStyle::View();
	Ref<YAMLResult> result = emitter.emit(input, style_view);
	return result;
}

Ref<YAMLResult> YAML::load_file(const String &path, const Ref<YAMLSecurity> security, const bool detect_style) {
	// Open file for reading
	Ref<FileAccess> file = FileAccess::open(path, FileAccess::READ);

	Error err = file->get_error();
	if (err != OK) {
		return YAMLResult::error("Failed to read '" + path + "': " + UtilityFunctions::error_string((int)err));
	}

	// Read the file content
	String content = file->get_as_text();

	// Check for read errors
	err = file->get_error();
	file->close();

	if (err != OK) {
		return YAMLResult::error("Failed to read '" + path + "': " + UtilityFunctions::error_string((int)err));
	}

	return parse(content, security, detect_style);
}

Ref<YAMLResult> YAML::save_file(const Variant &data, const String &path, const Ref<YAMLStyle> &style) {
	Ref<YAMLResult> stringify_result = stringify(data, style);
	if (stringify_result->has_error()) {
		return stringify_result;
	}

	String yaml_content = stringify_result->get_data();

	// Open file for writing
	Ref<FileAccess> file = FileAccess::open(path, FileAccess::WRITE);

	Error err = file->get_error();
	if (err != OK) {
		return YAMLResult::error("Failed to write '" + path + "': " + UtilityFunctions::error_string((int)err));
	}

	// Write YAML content to file
	file->store_string(yaml_content);

	// Check for write errors
	err = file->get_error();
	file->close();

	if (err != OK) {
		return YAMLResult::error("Failed to write '" + path + "': " + UtilityFunctions::error_string((int)err));
	}

	// Return success with the YAML content
	return YAMLResult::success(yaml_content);
}

Ref<YAMLStyle> YAML::create_style() {
	return Ref<YAMLStyle>(memnew(YAMLStyle()));
}

void YAML::register_class(Ref<Script> p_class, const Variant &p_serialize, const Variant &p_deserialize) {
	YAMLClassRegistry::register_class(p_class, p_serialize, p_deserialize);
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

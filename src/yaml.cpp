#include "yaml.h"
#include "class_registry.h"
#include "emitter.h"
#include "parser.h"
#include "result.h"
#include "security.h"
#include "style_view.h"
#include "validator.h"
#include "version.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include <sstream>

using namespace godot;

void YAML::_bind_methods() {
	ClassDB::bind_static_method("YAML", D_METHOD("version"), &YAML::version);
	ClassDB::bind_static_method("YAML", D_METHOD("validate", "input"), &YAML::validate);
	ClassDB::bind_static_method("YAML", D_METHOD("parse", "input", "detect_style", "security"), &YAML::parse, DEFVAL(false), DEFVAL(nullptr));
	ClassDB::bind_static_method("YAML", D_METHOD("stringify", "input", "style"), &YAML::stringify, DEFVAL(Variant()));

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

Ref<YAMLResult> YAML::parse(const String &input, const bool detect_style, const Ref<YAMLSecurity> security) {
	Parser parser;
	if (security.is_valid()) {
		return parser.parse(input, detect_style, security->get_view());
	} else {
		return parser.parse(input, detect_style, YAMLSecurity::get_default_view());
	}
}

Ref<YAMLResult> YAML::validate(const String &input) {
	Validator validator;
	Ref<YAMLResult> result = validator.validate(input);
	return result;
}

Ref<YAMLResult> YAML::stringify(const Variant &input, const Ref<YAMLStyle> &style) {
	Emitter emitter;
	YAMLStyle::View style_view = YAMLStyle::View::create_view(style);
	Ref<YAMLResult> result = emitter.emit(input, style_view);
	return result;
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

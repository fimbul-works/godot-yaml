#include "yaml.h"
#include "class_registry.h"
#include "emitter.h"
#include "parser.h"
#include "result.h"
#include "style_view.h"
#include "validator.h"
#include "version.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include <sstream>

using namespace godot;

void YAML::_bind_methods()
{
  ClassDB::bind_static_method("YAML", D_METHOD("version"), &YAML::version);
  ClassDB::bind_static_method("YAML", D_METHOD("validate", "input"), &YAML::validate);
  ClassDB::bind_static_method("YAML", D_METHOD("parse", "input", "detect_style"), &YAML::parse, DEFVAL(false));
  ClassDB::bind_static_method("YAML", D_METHOD("stringify", "input", "style"), &YAML::stringify, DEFVAL(Variant()));
  ClassDB::bind_static_method("YAML", D_METHOD("create_style"), &YAML::create_style);

  ClassDB::bind_static_method("YAML", D_METHOD("register_class", "script_class", "to_dict", "from_dict"), &YAML::register_class, DEFVAL("to_dict"), DEFVAL("from_dict"));
  ClassDB::bind_static_method("YAML", D_METHOD("has_registered_class", "tag_name"), &YAML::has_registered_class);
}

String YAML::version()
{
#ifdef GODOT_YAML_DEBUG
  String target = "debug";
#else
  String target = "release";
#endif
  return String("Godot YAML " + String(GODOT_YAML_VERSION) + " (" + target + ")");
}

Ref<YAMLResult> YAML::parse(const String& input, const bool detect_style)
{
  Parser parser;
  Ref<YAMLResult> result = parser.parse(input, detect_style);
  return result;
}

Ref<YAMLResult> YAML::validate(const String& input)
{
  Validator validator;
  Ref<YAMLResult> result = validator.validate(input);
  return result;
}

Ref<YAMLResult> YAML::stringify(const Variant& input, const Ref<YAMLStyle>& style)
{
  Emitter emitter;
  YAMLStyle::View style_view = YAMLStyle::View::create_view(style);
  Ref<YAMLResult> result = emitter.emit(input, style_view);
  return result;
}

Ref<YAMLStyle> YAML::create_style()
{
  return Ref<YAMLStyle>(memnew(YAMLStyle()));
}

void YAML::register_class(Ref<Script> p_class, const Variant& p_to_dict, const Variant& p_from_dict)
{
  YAMLClassRegistry::register_class(p_class, p_to_dict, p_from_dict);
}

bool YAML::has_registered_class(const String& class_name)
{
  return YAMLClassRegistry::has_class(class_name);
}

#include "yaml.h"
#include "emitter.h"
#include "parser.h"
#include "result.h"
#include "style_view.h"

#include "version.h"

using namespace godot;

void YAML::_bind_methods()
{
  ClassDB::bind_static_method("YAML", D_METHOD("version"), &YAML::version);
  ClassDB::bind_static_method("YAML", D_METHOD("parse", "input", "detect_style"), &YAML::parse, DEFVAL(false));
  ClassDB::bind_static_method("YAML", D_METHOD("emit", "input", "style"), &YAML::emit, DEFVAL(Variant()));
  ClassDB::bind_static_method("YAML", D_METHOD("create_style"), &YAML::create_style);
}

String YAML::version()
{
#ifdef GODOT_YAML_DEBUG
  String target = "debug";
#else
  String target = "release";
#endif
  return String("Version " + String(GODOT_YAML_VERSION) + " (" + target + ")");
}

Ref<YAMLResult> YAML::parse(const String& input, const bool detect_style)
{
  Parser parser;
  Ref<YAMLResult> result = parser.parse(input, detect_style);
  return result;
}

Ref<YAMLResult> YAML::emit(const Variant& input, const Ref<YAMLStyle>& style)
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

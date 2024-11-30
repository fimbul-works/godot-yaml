#include "yaml.h"
#include "emitter.h"
#include "parser.h"
#include "result.h"

#include "version.h"

using namespace godot;

void YAML::_bind_methods()
{
  ClassDB::bind_static_method("YAML", D_METHOD("version"), &YAML::version);
  ClassDB::bind_static_method("YAML", D_METHOD("parse", "input"), &YAML::parse);
  ClassDB::bind_static_method("YAML", D_METHOD("emit", "input", "format"), &YAML::emit, DEFVAL(format()));
  ClassDB::bind_static_method("YAML", D_METHOD("format"), &YAML::format);
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

Ref<YAMLResult> YAML::parse(const String& input)
{
  YAMLParser parser;
  Ref<YAMLResult> result = parser.parse(input);
  return result;
}

Ref<YAMLResult> YAML::emit(const Variant& input, const Ref<YAMLFormat>& format = YAML::format())
{
  YAMLEmitter emitter;
  Ref<YAMLResult> result = emitter.emit(input, format);
  return result;
}

Ref<YAMLFormat> YAML::format()
{
  return memnew(YAMLFormat);
}

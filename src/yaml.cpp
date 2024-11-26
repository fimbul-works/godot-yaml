#include "yaml.h"
#include "yaml_emitter.h"
#include "yaml_parser.h"
#include "yaml_result.h"

#include "version.h"

using namespace godot;

void YAML::_bind_methods()
{
  ClassDB::bind_static_method("YAML", D_METHOD("version"), &YAML::version);

  ClassDB::bind_static_method("YAML", D_METHOD("create_parser"), &YAML::create_parser);
  ClassDB::bind_static_method("YAML", D_METHOD("create_emitter"), &YAML::create_emitter);

  ClassDB::bind_static_method("YAML", D_METHOD("parse", "input"), &YAML::parse);
  ClassDB::bind_static_method("YAML", D_METHOD("emit", "input", "format"), &YAML::emit,
          DEFVAL(YAMLFormat::create_default()));
}

YAML::YAML() :
        Object() { };

YAML::~YAML() { };

String YAML::version()
{
#ifdef GODOT_YAML_DEBUG
  String target = "debug";
#else
  String target = "release";
#endif
  return String("Version " + String(GODOT_YAML_VERSION) + " (" + target + ")");
}

Ref<YAMLParser> YAML::create_parser()
{
  return Ref<YAMLParser>(memnew(YAMLParser()));
}

Ref<YAMLEmitter> YAML::create_emitter()
{
  return Ref<YAMLEmitter>(memnew(YAMLEmitter()));
}

Ref<YAMLResult> YAML::parse(const String& input)
{
  return create_parser()->parse(input);
}

Ref<YAMLResult> YAML::emit(const Variant& input, const Ref<YAMLFormat>& format)
{
  return create_emitter()->emit(input, format);
}

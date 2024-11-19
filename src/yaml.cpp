#include "yaml.h"
#include "version.h"

using namespace godot;

void YAML::_bind_methods()
{
  ClassDB::bind_method(D_METHOD("version"), &YAML::version);
  ClassDB::bind_method(D_METHOD("parse", "input"), &YAML::parse);
  ClassDB::bind_method(D_METHOD("emit", "input"), &YAML::emit);
  ClassDB::bind_method(D_METHOD("create_parser"), &YAML::create_parser);
  ClassDB::bind_method(D_METHOD("create_emitter"), &YAML::create_emitter);
}

YAML::YAML()
{
  // Create default parser and emitter instances
  default_parser = create_parser();
  default_emitter = create_emitter();
}

YAML::~YAML()
{
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
  return default_parser->parse(input);
}

Ref<YAMLResult> YAML::emit(const Variant& input)
{
  return default_emitter->emit(input);
}

Ref<YAMLParser> YAML::create_parser() const
{
  Ref<YAMLParser> parser;
  parser.instantiate();
  return parser;
}

Ref<YAMLEmitter> YAML::create_emitter() const
{
  Ref<YAMLEmitter> emitter;
  emitter.instantiate();
  return emitter;
}

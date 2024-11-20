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
  ClassDB::bind_method(D_METHOD("set_format", "type", "format"), &YAML::set_format);
  ClassDB::bind_method(D_METHOD("get_format", "type"), &YAML::get_format);
}

YAML::YAML()
{
  // Register all variant converters first
  register_type_converters();

  // Create default parser and emitter instances
  default_parser = create_parser();
  default_emitter = create_emitter();
}

YAML::~YAML() = default;

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

bool YAML::set_format(Variant::Type type, const String& format)
{
  return default_emitter->set_format(type, format);
}

String YAML::get_format(Variant::Type type) const
{
  return default_emitter->get_format(type);
}

const VariantConverter* YAML::get_converter_by_tag(const String& tag) const
{
  auto it = tag_to_converter.find(tag);
  return it != tag_to_converter.end() ? it->second : nullptr;
}

const VariantConverter* YAML::get_converter_by_type(Variant::Type type) const
{
  auto it = type_to_converter.find(type);
  return it != type_to_converter.end() ? it->second.get() : nullptr;
}

void YAML::register_converter(std::unique_ptr<VariantConverter> converter)
{
  tag_to_converter[converter->get_full_tag()] = converter.get();
  type_to_converter[converter->get_type()] = std::move(converter);
}

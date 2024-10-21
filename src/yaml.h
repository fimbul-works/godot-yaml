#ifndef YAML_H
#define YAML_H

#include "variants/resource_yaml.h"
#include "yaml_encoder.h"

#include <godot_cpp/core/object.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <ryml.hpp>
#include <ryml_std.hpp>

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace godot {

class YAMLEncoder;
class ResourceYAMLEncoder;

class YAMLException : public std::runtime_error {
  public:
  YAMLException(const String& msg) :
          std::runtime_error(msg.utf8().get_data()) { }
  YAMLException(const std::string& what_arg) :
          std::runtime_error(what_arg) { }
  YAMLException(const char* what_arg) :
          std::runtime_error(what_arg) { }
};

class YAML : public Object {
  GDCLASS(YAML, Object)

  protected:
  // Godot method binding
  static void _bind_methods();

  public:
  YAML();
  ~YAML();

  String version();

  // Error handling
  public:
  Variant get_error() const;

  private:
  static void error_callback(const char* msg, size_t len, ryml::Location loc, void* user_data);

  static std::string error;

  // YAML parsing
  public:
  Variant parse(const String& input);

  private:
  Variant yaml_to_variant(const ryml::ConstNodeRef& node);
  Variant parse_key(const ryml::ConstNodeRef& n);
  Variant parse_value(const ryml::ConstNodeRef& n);

  ryml::Callbacks m_callbacks;
  ryml::Tree m_tree;
  std::unique_ptr<ryml::EventHandlerTree> m_evt_handler;
  std::unique_ptr<ryml::Parser> m_parser;

  // YAML emitting
  public:
  String stringify(const Variant& input);

  private:
  void YAML::emit_recursively(ryml::NodeRef& node, const Variant& v);

  // YAML encoders
  public:
  bool set_format(Variant::Type type, const String& format);

  private:
  void register_encoder(std::unique_ptr<YAMLEncoder> encoder);
  void register_type_encoders();

  std::unordered_map<std::string, YAMLEncoder*> tag_to_encoder;
  std::unordered_map<Variant::Type, std::unique_ptr<YAMLEncoder>> type_to_encoder;
  std::unique_ptr<ResourceYAMLEncoder> resource_encoder;
};

} // namespace godot

#endif // YAML_H

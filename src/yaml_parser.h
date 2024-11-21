// yaml_parser.h
#ifndef YAML_PARSER_H
#define YAML_PARSER_H

#include "yaml_result.h"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <ryml.hpp>

#include <memory>
#include <optional>

namespace godot {

class YAMLParser : public RefCounted {
  GDCLASS(YAMLParser, RefCounted)

  protected:
  static void _bind_methods();

  public:
  YAMLParser();
  ~YAMLParser();

  Ref<YAMLResult> parse(const String& input);

  private:
  // Error handling
  static void error_callback(const char* msg, size_t len, ryml::Location loc, void* user_data);
  Ref<YAMLResult> current_result;

  // YAML parsing state
  ryml::Callbacks m_callbacks;
  ryml::Tree m_tree;
  std::unique_ptr<ryml::EventHandlerTree> m_evt_handler;
  std::unique_ptr<ryml::Parser> m_parser;

  // YAML parsing helpers
  Variant yaml_to_variant(const ryml::ConstNodeRef& node);
  Variant parse_map(const ryml::ConstNodeRef& node);
  Variant parse_sequence(const ryml::ConstNodeRef& node);
  Variant parse_key(const ryml::ConstNodeRef& node);
  Variant parse_value(const ryml::ConstNodeRef& node);
  std::optional<Variant> parse_special_value(const String& str_val);
  std::optional<Variant> parse_numeric_value(const String& str_val, const ryml::csubstr& val);
};

} // namespace godot

#endif // YAML_PARSER_H

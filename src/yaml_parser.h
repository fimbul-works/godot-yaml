#ifndef YAML_PARSER_H
#define YAML_PARSER_H

#include "variant_converter_registry.h"
#include "yaml_format.h"
#include "yaml_result.h"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <ryml.hpp>

#include <memory>
#include <mutex>
#include <optional>

namespace godot {

class YAMLParser : public RefCounted {
  GDCLASS(YAMLParser, RefCounted)

  protected:
  static void _bind_methods();

  public:
  YAMLParser();
  virtual ~YAMLParser();

  Ref<YAMLResult> parse(const String& input);

  private:
  // Error handling
  static void error_callback(const char* msg, size_t len, ryml::Location loc, void* user_data);

  // Thread safety
  std::mutex parse_mutex;

  // YAML parsing setup
  ryml::Callbacks m_callbacks;
  std::unique_ptr<ryml::EventHandlerTree> m_evt_handler;
  std::unique_ptr<ryml::Parser> m_parser;
  ryml::Tree m_tree;
  Ref<YAMLResult> current_result;

  // Core parsing methods
  Variant process_node(const ryml::ConstNodeRef& node) const;
  Variant process_map(const ryml::ConstNodeRef& node) const;
  Variant process_sequence(const ryml::ConstNodeRef& node) const;
  Variant process_key(const ryml::ConstNodeRef& node) const;
  Variant process_value(const ryml::ConstNodeRef& node) const;

  // Value parsing helpers
  std::optional<Variant> try_parse_tagged_value(const ryml::ConstNodeRef& node) const;
  std::optional<Variant> try_parse_special_value(const String& str_val) const;
  std::optional<Variant> try_parse_numeric_value(const String& str_val, const ryml::csubstr& val) const;
  String extract_tag(const ryml::ConstNodeRef& node) const;
};

} // namespace godot

#endif // YAML_PARSER_H

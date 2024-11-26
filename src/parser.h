#ifndef YAML_PARSER_H
#define YAML_PARSER_H

#include "format.h"
#include "result.h"
#include "variant_converter_registry.h"

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
  virtual ~YAMLParser();
  Ref<YAMLResult> parse(const String& input);

  private:
  // Thread-safe private instance
  struct ParserInstance {
    ryml::Callbacks m_callbacks;
    std::unique_ptr<ryml::EventHandlerTree> m_evt_handler;
    std::unique_ptr<ryml::Parser> m_parser;
    ryml::Tree m_tree;
    Ref<YAMLResult> current_result;

    ParserInstance();
    ~ParserInstance();

    static void error_callback(const char* msg, size_t len, ryml::Location loc, void* user_data);

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

  static thread_local ParserInstance t_parser_instance;
};

} // namespace godot

#endif // YAML_PARSER_H

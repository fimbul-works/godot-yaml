#ifndef YAML_PARSER_H
#define YAML_PARSER_H

#include "result.h"
#include "style.h"
#include "yaml.h"

#include <ryml.hpp>

#include <memory>
#include <optional>
#include <vector>

namespace godot {

class YAML::Parser {
  public:
  Parser();
  ~Parser() = default;

  Ref<YAMLResult> parse(const String& input, const bool detect_style = false);

  private:
  // Parser state
  ryml::Callbacks callbacks;
  std::unique_ptr<ryml::EventHandlerTree> evt_handler;
  std::unique_ptr<ryml::Parser> parser;
  ryml::Tree tree;

  // Current state
  Ref<YAMLResult> current_result;
  std::vector<std::string> current_path;
  bool detect_style;
  Ref<YAMLStyle> style;

  // Error handling
  static void error_callback(const char* msg, size_t len, ryml::Location loc, void* user_data);

  // Node processing methods
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

  // Style detection methods
  void detect_node_style(const ryml::ConstNodeRef& node);
  void detect_scalar_style(const ryml::ConstNodeRef& node, const Ref<YAMLStyle>& style);
  void detect_container_form(const ryml::ConstNodeRef& node, const Ref<YAMLStyle>& style);
  void detect_anchor_style(const ryml::ConstNodeRef& node, const Ref<YAMLStyle>& style);
};

} // namespace godot

#endif // YAML_PARSER_H

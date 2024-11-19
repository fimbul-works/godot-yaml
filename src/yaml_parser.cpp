#include "yaml_parser.h"
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void YAMLParser::_bind_methods()
{
  ClassDB::bind_method(D_METHOD("parse", "input"), &YAMLParser::parse);
}

YAMLParser::YAMLParser()
{
  m_parser = std::make_unique<ryml::Parser>();

  // Set up error callback
  ryml::Callbacks callbacks;
  callbacks.m_error = error_callback;
  callbacks.m_user_data = this;
  ryml::set_callbacks(callbacks);
}

YAMLParser::~YAMLParser() = default;

void YAMLParser::error_callback(const char* msg, size_t len, ryml::Location loc, void* user_data)
{
  auto parser = static_cast<YAMLParser*>(user_data);
  if (parser && parser->current_result.is_valid()) {
    parser->current_result->set_error(String::utf8(msg, len), loc.line, loc.col);
  }
}

Ref<YAMLResult> YAMLParser::parse(const String& input)
{
  current_result.instantiate();

  try {
    // Parse YAML
    m_tree.clear();
    ryml::parse_in_arena(m_parser.get(), input.utf8().get_data(), &m_tree);

    // Check for empty document
    if (m_tree.empty()) {
      current_result->set_error("Empty YAML document");
      ERR_PRINT("YAML Parse Error: Empty document");
      return current_result;
    }

    // Resolve aliases and references
    m_tree.resolve();

    // Store the parsed data in the result
    current_result->data = yaml_to_variant(m_tree.rootref());
    return current_result;
  } catch (const std::exception& e) {
    current_result->set_error(e.what());
    ERR_PRINT(String("YAML Parse Error: ") + e.what());
    return current_result;
  }
}

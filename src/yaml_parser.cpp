#include "yaml_parser.h"
#include "util_numeric.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void YAMLParser::_bind_methods()
{
  ClassDB::bind_method(D_METHOD("parse", "input"), &YAMLParser::parse);
}

YAMLParser::YAMLParser()
{
  m_callbacks.m_error = error_callback;
  m_callbacks.m_user_data = this;
  m_evt_handler = std::make_unique<ryml::EventHandlerTree>(m_callbacks);
  m_parser = std::make_unique<ryml::Parser>(m_evt_handler.get(),
          ryml::ParserOptions().locations(true));
  ryml::set_callbacks(m_callbacks);
}

YAMLParser::~YAMLParser() = default;

void YAMLParser::error_callback(const char* msg, size_t len, ryml::Location loc, void* user_data)
{
  auto* parser = static_cast<YAMLParser*>(user_data);
  if (!parser) {
    return;
  }

  std::lock_guard<std::mutex> lock(parser->parse_mutex);
  parser->current_result = YAMLResult::error(String::utf8(msg, len), loc.line, loc.col);
}

Ref<YAMLResult> YAMLParser::parse(const String& input)
{
  std::lock_guard<std::mutex> lock(parse_mutex);

  current_result = YAMLResult::success(Variant());

  try {
    m_tree.clear();
    ryml::parse_in_arena(m_parser.get(), input.utf8().get_data(), &m_tree);

    if (m_tree.empty()) {
      return YAMLResult::error("Empty YAML document");
    }

    m_tree.resolve();

    if (!current_result->has_error()) {
      current_result = YAMLResult::success(process_node(m_tree.rootref()));
    }

    return current_result;

  } catch (const std::exception& e) {
    return YAMLResult::error(e.what());
  }
}

Variant YAMLParser::process_node(const ryml::ConstNodeRef& node) const
{
  try {
    // First check for tagged values
    if (auto tagged = try_parse_tagged_value(node)) {
      return *tagged;
    }

    // Handle different node types
    if (node.is_keyval()) {
      return process_value(node);
    } else if (node.is_map()) {
      return process_map(node);
    } else if (node.is_seq()) {
      return process_sequence(node);
    } else if (node.has_key()) {
      return process_key(node);
    } else if (node.has_val()) {
      return process_value(node);
    }

    return Variant();
  } catch (const std::exception& e) {
    ERR_PRINT(String("YAML parsing error: ") + e.what());
    return Variant();
  }
}

Variant YAMLParser::process_map(const ryml::ConstNodeRef& node) const
{
  Dictionary dict;

  for (const auto& child : node.children()) {
    Variant key = process_key(child);
    if (key.get_type() == Variant::NIL && current_result->has_error()) {
      return Variant();
    }

    Variant value = process_node(child);
    if (value.get_type() == Variant::NIL && current_result->has_error()) {
      return Variant();
    }

    dict[key] = value;
  }

  return dict;
}

Variant YAMLParser::process_sequence(const ryml::ConstNodeRef& node) const
{
  Array arr;

  for (const auto& child : node.children()) {
    Variant item = process_node(child);
    if (item.get_type() == Variant::NIL && current_result->has_error()) {
      return Variant();
    }
    arr.push_back(item);
  }

  return arr;
}

Variant YAMLParser::process_key(const ryml::ConstNodeRef& node) const
{
  if (!node.has_key()) {
    return Variant();
  }
  return String::utf8(node.key().str, node.key().len);
}

Variant YAMLParser::process_value(const ryml::ConstNodeRef& node) const
{
  // Handle null/empty values
  if (!node.has_val() || node.val().empty() || node.val_is_null()) {
    return Variant();
  }

  try {
    ryml::csubstr val = node.val();
    String str_val = String::utf8(val.str, val.len);

    // Handle special values first
    if (auto special_val = try_parse_special_value(str_val)) {
      return *special_val;
    }

    // Try numeric conversion
    if (auto num_val = try_parse_numeric_value(str_val, val)) {
      return *num_val;
    }

    // Return as string if no other type matches
    return str_val;
  } catch (const std::exception& e) {
    ERR_PRINT(String("Value parsing error: ") + e.what());
    return Variant();
  }
}

std::optional<Variant> YAMLParser::try_parse_special_value(const String& str_val) const
{
  // Boolean values
  if (str_val == "true")
    return true;
  if (str_val == "false")
    return false;

  // Null values
  if (str_val == "null" || str_val == "~")
    return Variant();

  // Special float values
  if (str_val == ".inf" || str_val == "+.inf")
    return Math_INF;
  if (str_val == "-.inf")
    return -Math_INF;
  if (str_val == ".nan")
    return Math_NAN;

  return std::nullopt;
}

std::optional<Variant> YAMLParser::try_parse_numeric_value(const String& str_val, const ryml::csubstr& val) const
{
  // Handle different integer formats
  if (str_val.begins_with("0b") || str_val.begins_with("0B") || // Binary
          str_val.begins_with("0o") || str_val.begins_with("0O") || // Octal
          str_val.begins_with("0x") || str_val.begins_with("0X") || // Hexadecimal
          (str_val.length() > 1 && str_val[0] == '0' && str_val[1] >= '0' && str_val[1] <= '7')) // Octal
  {
    try {
      return string_to_int<int64_t>(val);
    } catch (const std::exception& e) {
      return std::nullopt;
    }
  }

  // Handle decimal numbers
  if (str_val.is_valid_int()) {
    try {
      return string_to_int<int64_t>(val);
    } catch (const std::exception& e) {
      return std::nullopt;
    }
  }

  if (str_val.is_valid_float()) {
    try {
      return string_to_float<double>(val);
    } catch (const std::exception& e) {
      return std::nullopt;
    }
  }

  return std::nullopt;
}

std::optional<Variant> YAMLParser::try_parse_tagged_value(const ryml::ConstNodeRef& node) const
{
  String tag = extract_tag(node);
  if (tag.is_empty()) {
    return std::nullopt;
  }

  const VariantConverter* converter = VariantConverterRegistry::get_converter_by_tag(tag);
  if (!converter) {
    return std::nullopt;
  }

  return converter->decode(node);
}

String YAMLParser::extract_tag(const ryml::ConstNodeRef& node) const
{
  if (!node.has_val_tag()) {
    return String();
  }

  const auto& tag = node.val_tag();
  if (tag.begins_with('!')) {
    return String::utf8(tag.sub(1).str, tag.len - 1);
  }
  return String::utf8(tag.str, tag.len);
}

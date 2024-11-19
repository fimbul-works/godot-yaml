#include "util_numeric.h"
#include "yaml_parser.h"
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

Variant YAMLParser::yaml_to_variant(const ryml::ConstNodeRef& n)
{
  try {
    // Handle different node types
    if (n.is_keyval()) {
      return parse_value(n);
    } else if (n.is_map()) {
      return parse_map(n);
    } else if (n.is_seq()) {
      return parse_sequence(n);
    } else if (n.has_key()) {
      return parse_key(n);
    } else if (n.has_val()) {
      return parse_value(n);
    }

    current_result->set_error("Unhandled YAML node type");
    return Variant();
  } catch (const std::exception& e) {
    current_result->set_error(e.what());
    return Variant();
  }
}

Variant YAMLParser::parse_map(const ryml::ConstNodeRef& n)
{
  Dictionary dict;

  for (const auto& child : n.children()) {
    Variant key = parse_key(child);
    if (key.get_type() == Variant::NIL && current_result->has_error()) {
      return Variant();
    }

    Variant value = yaml_to_variant(child);
    if (value.get_type() == Variant::NIL && current_result->has_error()) {
      return Variant();
    }

    dict[key] = value;
  }

  return dict;
}

Variant YAMLParser::parse_sequence(const ryml::ConstNodeRef& n)
{
  Array arr;

  for (const auto& child : n.children()) {
    Variant item = yaml_to_variant(child);
    if (item.get_type() == Variant::NIL && current_result->has_error()) {
      return Variant();
    }
    arr.push_back(item);
  }

  return arr;
}

Variant YAMLParser::parse_key(const ryml::ConstNodeRef& n)
{
  if (!n.has_key()) {
    current_result->set_error("Invalid key in YAML document");
    return Variant();
  }

  return String::utf8(n.key().str, n.key().len);
}

Variant YAMLParser::parse_value(const ryml::ConstNodeRef& n)
{
  // Handle null/empty values
  if (!n.has_val() || n.val().empty() || n.val_is_null()) {
    return Variant();
  }

  try {
    ryml::csubstr val = n.val();
    String str_val = String::utf8(val.str, val.len);

    // Handle special values
    if (auto special_val = parse_special_value(str_val); special_val.has_value()) {
      return special_val.value();
    }

    // Handle numbers
    if (auto num_val = parse_numeric_value(str_val, val); num_val.has_value()) {
      return num_val.value();
    }

    // Return as string if no other type matches
    return str_val;
  } catch (const std::exception& e) {
    current_result->set_error(e.what());
    return Variant();
  }
}

std::optional<Variant> YAMLParser::parse_special_value(const String& str_val)
{
  // Boolean values
  if (str_val == "true")
    return Variant(true);
  if (str_val == "false")
    return Variant(false);

  // Null values
  if (str_val == "null" || str_val == "~")
    return Variant();

  // Special float values
  if (str_val == ".inf" || str_val == "+.inf")
    return Variant(Math_INF);
  if (str_val == "-.inf")
    return Variant(-Math_INF);
  if (str_val == ".nan")
    return Variant(Math_NAN);

  return std::nullopt;
}

std::optional<Variant> YAMLParser::parse_numeric_value(const String& str_val, const ryml::csubstr& val)
{
  // Handle different integer formats
  if (str_val.begins_with("0b") || str_val.begins_with("0B") || // Binary
          str_val.begins_with("0o") || str_val.begins_with("0O") || // Octal
          str_val.begins_with("0x") || str_val.begins_with("0X") || // Hexadecimal
          (str_val.length() > 1 && str_val[0] == '0' && str_val[1] >= '0' && str_val[1] <= '7')) // Octal
  {
    return Variant(string_to_int<int64_t>(val));
  }

  // Handle decimal numbers
  if (str_val.is_valid_int()) {
    return Variant(string_to_int<int64_t>(val));
  }
  if (str_val.is_valid_float()) {
    return Variant(string_to_float<double>(val));
  }

  return std::nullopt;
}

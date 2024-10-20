#include "yaml.h"

#include "util_numeric.h"

using namespace godot;

Variant YAML::parse(const String& input)
{
  std::string error_prefix = "YAML parse error: ";
  error = "";

  try {
    m_tree.clear();
    ryml::parse_in_arena(m_parser.get(), input.utf8().get_data(), &m_tree);

    if (m_tree.empty()) {
      error = "empty YAML document";
      UtilityFunctions::printerr(error_prefix.c_str(), error.c_str());
      return Variant();
    }

    // Resolve aliases etc
    m_tree.resolve();

    return yaml_to_variant(m_tree.rootref());
  } catch (const YAMLException& e) {
    UtilityFunctions::printerr(error_prefix.c_str(), e.what());
    return Variant();
  } catch (const std::exception& e) {
    UtilityFunctions::printerr(error_prefix.c_str(), e.what());
    return Variant();
  } catch (...) {
    UtilityFunctions::printerr(error_prefix.c_str(), "unknown error");
    return Variant();
  }
}

Variant YAML::yaml_to_variant(const ryml::ConstNodeRef& n)
{
  if (n.has_val_tag()) {
    std::string tag = std::string(n.val_tag().str, n.val_tag().len);

    // Handle resource
    if (tag == resource_encoder->get_full_tag()) {
      return resource_encoder->decode(n);
    }

    // Look up other Variants
    auto it = tag_to_encoder.find(tag);
    if (it != tag_to_encoder.end()) {
      return it->second->decode(n);
    }
  }

  if (n.is_keyval()) {
    return parse_value(n);
  } else if (n.is_map()) {
    Dictionary dict;

    for (const auto& child : n.children()) {
      Variant key = parse_key(child);

      if (key.get_type() == Variant::NIL) {
        return Variant();
      }

      Variant value = yaml_to_variant(child);

      if (value.get_type() == Variant::NIL && !error.empty()) {
        return Variant();
      }

      dict[key] = value;
    }

    return dict;
  } else if (n.is_seq()) {
    Array arr;

    for (const auto& child : n.children()) {
      Variant item = yaml_to_variant(child);

      if (item.get_type() == Variant::NIL && !error.empty()) {
        return Variant();
      }

      arr.push_back(item);
    }

    return arr;
  } else if (n.has_key()) {
    return parse_key(n);
  } else if (n.has_val()) {
    return parse_value(n);
  } else {
    std::string error_msg = "unhandled YAML node type";
    error_callback(error_msg.c_str(), error_msg.length(), m_parser->location(n), nullptr);
    return Variant();
  }
}

Variant YAML::parse_key(const ryml::ConstNodeRef& n)
{
  if (n.has_key()) {
    return String::utf8(n.key().str, n.key().len);
  }

  std::string key_error = "invalid key";
  error_callback(key_error.c_str(), key_error.length(), m_parser->location(n), nullptr);
  return Variant();
}

Variant YAML::parse_value(const ryml::ConstNodeRef& n)
{
  // Null
  if (!n.has_val() || n.val().empty() || n.val_is_null()) {
    return Variant();
  }

  ryml::csubstr val = n.val();
  String str_val = String::utf8(val.str, val.len);

  std::string tag = n.has_val_tag() ? std::string(n.val_tag().str) : "";

  // Boolean
  if (str_val == "true")
    return true;
  if (str_val == "false")
    return false;

  // Null string
  if (str_val == "null" || str_val == "~")
    return Variant();

  // Special float values
  if (str_val == ".inf" || str_val == "+.inf")
    return Math_INF;
  if (str_val == "-.inf")
    return -Math_INF;
  if (str_val == ".nan")
    return Math_NAN;

  // Handle binary integers
  if (str_val.begins_with("0b") || str_val.begins_with("0B")) {
    return string_to_int<int64_t>(val);
  }

  // Handle octal integers
  if (str_val.begins_with("0o") || str_val.begins_with("0O") || (str_val.length() > 1 && str_val[0] == '0' && str_val[1] >= '0' && str_val[1] <= '7')) {
    return string_to_int<int64_t>(val);
  }

  // Handle hexadecimal integers
  if (str_val.begins_with("0x") || str_val.begins_with("0X")) {
    return string_to_int<int64_t>(val);
  }

  // Handle decimal integers and floats
  if (str_val.is_valid_int())
    return string_to_int<int64_t>(val);
  if (str_val.is_valid_float())
    return string_to_float<double>(val);

  return str_val;
}

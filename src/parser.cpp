#include "parser.h"
#include "util_numeric.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

thread_local YAMLParser::ParserInstance YAMLParser::t_parser_instance;

Ref<YAMLResult> YAMLParser::parse(const String& input, const bool detect_style)
{
  try {
    auto& instance = t_parser_instance;
    instance.detect_style = detect_style;
    instance.style = detect_style ? YAML::create_style() : nullptr;
    instance.current_result = YAMLResult::success(Variant());
    instance.m_tree.clear();

    ryml::parse_in_arena(
            instance.m_parser.get(),
            input.utf8().get_data(),
            &instance.m_tree);

    if (instance.m_tree.empty()) {
      return YAMLResult::error("Empty YAML document");
    }

    instance.m_tree.resolve();

    if (detect_style) {
      instance.detect_node_style(instance.m_tree.rootref());
    }

    if (!instance.current_result->has_error()) {
      Variant parsed_data = instance.process_node(instance.m_tree.rootref());
      instance.current_result = YAMLResult::success(parsed_data, instance.style);
    }

    return instance.current_result;

  } catch (const YAMLException& e) {
    return YAMLResult::error(e.what());
  } catch (const std::exception& e) {
    return YAMLResult::error(e.what());
  } catch (...) {
    return YAMLResult::error("Unknown error occurred during parsing");
  }
}

YAMLParser::ParserInstance::ParserInstance()
{
  m_callbacks.m_error = error_callback;
  m_callbacks.m_user_data = this;
  m_evt_handler = std::make_unique<ryml::EventHandlerTree>(m_callbacks);
  m_parser = std::make_unique<ryml::Parser>(m_evt_handler.get(), ryml::ParserOptions().locations(true));
}

void YAMLParser::ParserInstance::error_callback(const char* msg, size_t len, ryml::Location loc, void* user_data)
{
  ryml::csubstr error_msg(msg, len);

  // Strip "ERROR: " prefix if available
  const ryml::csubstr strip_error_prefix = "ERROR: ";
  if (error_msg.begins_with(strip_error_prefix)) {
    error_msg = error_msg.sub(strip_error_prefix.len);
  }

  // RapidYAML does not like complex keys
  if (error_msg.begins_with("ryml trees cannot handle containers as keys")) {
    error_msg = ryml::to_csubstr("unsupported complex key");
  }

  // Only return the first line, which has the relevent error message
  size_t newline_pos = error_msg.find('\n');
  if (newline_pos != ryml::substr::npos) {
    error_msg = error_msg.sub(0, newline_pos);
  }

  // Attempt to retrieve parser instance
  auto* instance = static_cast<ParserInstance*>(user_data);
  if (!instance) {
    throw YAMLException(String::utf8(error_msg.str, error_msg.len));
  }

  // Set the error object
  instance->current_result = YAMLResult::error(String::utf8(error_msg.str, error_msg.len), loc.line, loc.col);

  // Error handler MUST throw!
  throw YAMLException(instance->current_result->get_error());
}

Variant YAMLParser::ParserInstance::process_node(const ryml::ConstNodeRef& node) const
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

Variant YAMLParser::ParserInstance::process_map(const ryml::ConstNodeRef& node) const
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

Variant YAMLParser::ParserInstance::process_sequence(const ryml::ConstNodeRef& node) const
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

Variant YAMLParser::ParserInstance::process_key(const ryml::ConstNodeRef& node) const
{
  if (!node.has_key()) {
    return Variant();
  }
  return String::utf8(node.key().str, node.key().len);
}

Variant YAMLParser::ParserInstance::process_value(const ryml::ConstNodeRef& node) const
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

std::optional<Variant> YAMLParser::ParserInstance::try_parse_special_value(const String& str_val) const
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

std::optional<Variant> YAMLParser::ParserInstance::try_parse_numeric_value(const String& str_val, const ryml::csubstr& val) const
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

std::optional<Variant> YAMLParser::ParserInstance::try_parse_tagged_value(const ryml::ConstNodeRef& node) const
{
  String tag = extract_tag(node);
  if (tag.is_empty()) {
    return std::nullopt;
  }

  if (tag == "!binary") {
    const auto* converter = VariantConverterRegistry::get_converter_by_tag("PackedByteArray");
    if (converter) {
      return converter->decode(node);
    }
  }

  const VariantConverter* converter = VariantConverterRegistry::get_converter_by_tag(tag);
  if (!converter) {
    return std::nullopt;
  }

  return converter->decode(node);
}

String YAMLParser::ParserInstance::extract_tag(const ryml::ConstNodeRef& node) const
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

void YAMLParser::ParserInstance::detect_node_style(const ryml::ConstNodeRef& node)
{
  if (!detect_style || !style.is_valid()) {
    return;
  }

  // Get or create style for current path
  Ref<YAMLStyle> current_style;

  if (current_path.empty()) {
    current_style = style; // Use root style
  } else {
    // Navigate to current path
    current_style = style;
    for (const auto& path_element : current_path) {
      Ref<YAMLStyle> child = current_style->get_child(String(path_element.c_str()));
      if (!child.is_valid()) {
        child.instantiate();
        current_style->set_child(String(path_element.c_str()), child);
      }
      current_style = child;
    }
  }

  // Detect styles for this node
  detect_scalar_style(node, current_style);
  detect_collection_style(node, current_style);
  detect_binary_style(node, current_style);
  detect_anchor_style(node, current_style);

  // For map nodes, process children with updated path
  if (node.is_map()) {
    for (const auto& child : node.children()) {
      std::string key(child.key().str, child.key().len);
      auto new_path = current_path;
      new_path.push_back(key);
      current_path = new_path;
      detect_node_style(child);
      current_path.pop_back();
    }
  }
  // For sequence nodes, use indices as path elements
  else if (node.is_seq()) {
    int index = 0;
    for (const auto& child : node.children()) {
      auto new_path = current_path;
      new_path.push_back(std::to_string(index++));
      current_path = new_path;
      detect_node_style(child);
      current_path.pop_back();
    }
  }
}

void YAMLParser::ParserInstance::detect_scalar_style(const ryml::ConstNodeRef& node, const Ref<YAMLStyle>& style)
{
  if (!node.has_val()) {
    return;
  }

  // Handle quoted styles
  if (node.is_quoted()) {
    style->scalar_style = YAMLStyle::STYLE_QUOTED;
  }
  // Handle block styles
  else if (node.is_block() || node.is_val_folded()) {
    style->scalar_style = YAMLStyle::STYLE_BLOCK;
    style->block_style = node.is_val_folded() ? YAMLStyle::BLOCK_FOLDED : YAMLStyle::BLOCK_LITERAL;
  } else {
    style->scalar_style = YAMLStyle::STYLE_PLAIN;
  }

  // Detect number format
  std::string value_str(node.val().str, node.val().len);
  String value(value_str.c_str());
  if (value.is_valid_float() || value.is_valid_int()) {
    if (value.begins_with("0x") || value.begins_with("0X")) {
      style->number_format = YAMLStyle::NUM_HEX;
    } else if (value.begins_with("0o") || value.begins_with("0O")) {
      style->number_format = YAMLStyle::NUM_OCTAL;
    } else if (value.begins_with("0b") || value.begins_with("0B")) {
      style->number_format = YAMLStyle::NUM_BINARY;
    } else if (value.find("e") != -1 || value.find("E") != -1) {
      style->number_format = YAMLStyle::NUM_SCIENTIFIC;
    } else {
      style->number_format = YAMLStyle::NUM_DECIMAL;
    }
  }
}

void YAMLParser::ParserInstance::detect_collection_style(const ryml::ConstNodeRef& node, const Ref<YAMLStyle>& style)
{
  if (node.is_seq()) {
    style->collection_style = node.is_flow() ? YAMLStyle::COLLECTION_FLOW : YAMLStyle::COLLECTION_BLOCK;
  } else if (node.is_map()) {
    style->collection_style = node.is_flow() ? YAMLStyle::MAP_FLOW : YAMLStyle::MAP_BLOCK;
  }
}

void YAMLParser::ParserInstance::detect_binary_style(const ryml::ConstNodeRef& node, const Ref<YAMLStyle>& style)
{
  if (!node.has_val() || !node.has_val_tag()) {
    return;
  }

  // Detect binary encoding from tag
  if (node.val_tag() == "!!binary") {
    std::string value_str(node.val().str, node.val().len);
    String value(value_str.c_str());

    // Check if it's base64 encoded
    bool is_base64 = true;
    for (int i = 0; i < value.length(); i++) {
      char32_t c = value[i];
      if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '+' || c == '/' || c == '=' || c == '\n')) {
        is_base64 = false;
        break;
      }
    }

    style->binary_encoding = is_base64 ? YAMLStyle::BINARY_BASE64 : YAMLStyle::BINARY_HEX;

    // Set block style for binary data
    if (node.is_block()) {
      style->scalar_style = YAMLStyle::STYLE_BLOCK;
      style->block_style = YAMLStyle::BLOCK_LITERAL;
    }

    // Store the tag in custom settings
    Dictionary custom;
    custom["tag"] = "!!binary";
    style->custom_settings = custom;
  }
}

void YAMLParser::ParserInstance::detect_anchor_style(const ryml::ConstNodeRef& node, const Ref<YAMLStyle>& style)
{
  Dictionary custom_settings;

  // Check for anchors and aliases
  if (node.has_anchor()) {
    custom_settings["anchor"] = String::utf8(node.val_anchor().str, node.val_anchor().len);
  }

  if (node.is_ref()) {
    custom_settings["alias"] = String::utf8(node.val_ref().str, node.val_ref().len);
  }

  // Store custom settings if any were detected
  if (!custom_settings.is_empty()) {
    style->custom_settings = custom_settings;
  }
}

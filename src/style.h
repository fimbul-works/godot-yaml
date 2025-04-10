#ifndef YAML_STYLE_H
#define YAML_STYLE_H

#include "string_hash.h"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <ryml.hpp>

#include <memory>
#include <unordered_map>

namespace godot {

// Forward declarations
class YAMLStyle : public RefCounted {
  GDCLASS(YAMLStyle, RefCounted);

  public:
  class View;

  enum ScalarStyle {
    SCALAR_ANY, // Let emitter decide
    SCALAR_PLAIN, // Regular unquoted scalar
    SCALAR_BLOCK, // Basic block scalar
    SCALAR_LITERAL, // | style - preserve newlines
    SCALAR_FOLDED // > style - fold newlines
  };

  enum QuoteStyle {
    QUOTE_ANY, // Let emitter decide
    QUOTE_NONE, // Unquoted when possible
    QUOTE_SINGLE, // Force single quotes
    QUOTE_DOUBLE // Force double quotes
  };

  enum ContainerForm {
    FORM_ANY, // Let emitter decide
    FORM_SEQ, // List/array style
    FORM_MAP // Dictionary/map style
  };

  enum FlowStyle {
    FLOW_ANY, // Let emitter decide
    FLOW_NONE, // Block style
    FLOW_SINGLE, // Compact [a,b] or {k:v} style
  };

  enum NumberFormat {
    NUM_ANY, // Let emitter decide
    NUM_DECIMAL, // Standard decimal
    NUM_HEX, // Hexadecimal (0xFF)
    NUM_OCTAL, // Octal (0o700)
    NUM_BINARY, // Binary (0b1010)
    NUM_SCIENTIFIC // Scientific (1.23e+4)
  };

  enum BinaryEncoding {
    BIN_ANY, // Let emitter decide
    BIN_STRING, // Plain string
    BIN_BASE64, // Base64 encoded
    BIN_HEX // Hexadecimal encoded
  };

  YAMLStyle();

  // Style setters/getters
  bool has_scalar_style = false;
  void set_scalar_style(ScalarStyle p_style)
  {
    scalar_style = p_style;
    has_scalar_style = true;
  }
  ScalarStyle get_scalar_style() const { return scalar_style; }

  bool has_quote_style = false;
  void set_quote_style(QuoteStyle p_style)
  {
    quote_style = p_style;
    has_quote_style = true;
  }
  QuoteStyle get_quote_style() const { return quote_style; }

  bool has_container_form = false;
  void set_container_form(ContainerForm p_style)
  {
    container_form = p_style;
    has_container_form = true;
  }
  ContainerForm get_container_form() const { return container_form; }

  bool has_flow_style = false;
  void set_flow_style(FlowStyle p_style)
  {
    flow_style = p_style;
    has_flow_style = true;
  }
  FlowStyle get_flow_style() const { return flow_style; }

  bool has_number_format = false;
  void set_number_format(NumberFormat p_format)
  {
    number_format = p_format;
    has_number_format = true;
  }
  NumberFormat get_number_format() const { return number_format; }

  bool has_binary_encoding = false;
  void set_binary_encoding(BinaryEncoding p_encoding)
  {
    binary_encoding = p_encoding;
    has_binary_encoding = true;
  }
  BinaryEncoding get_binary_encoding() const { return binary_encoding; }

  bool has_custom_settings = false;
  void set_custom_settings(Dictionary p_custom)
  {
    custom_settings = p_custom;
    has_custom_settings = true;
  }
  Dictionary get_custom_settings() const { return custom_settings; }

  void set_custom_tag(const String& p_tag)
  {
    custom_settings["tag"] = p_tag;
  }
  String get_custom_tag() const
  {
    Dictionary custom = get_custom_settings();
    if (custom.has("tag"))
      return custom["tag"];
    return "";
  }

  // Helper methods
  bool is_block_style() const;
  bool uses_quotes() const;
  bool uses_flow() const;

  // Child style management
  Ref<YAMLStyle> get_child(const String& key) const;
  void set_child(const String& key, const Ref<YAMLStyle>& style);
  bool has_child(const String& key) const;
  void clear_child(const String& key);
  void clear_children();
  Array get_children_keys() const;

  // Debug helper
  String get_debug_string() const;

  // Custom settings
  Dictionary custom_settings;

  protected:
  static void _bind_methods();

  private:
  ScalarStyle scalar_style;
  QuoteStyle quote_style;
  ContainerForm container_form;
  FlowStyle flow_style;
  NumberFormat number_format;
  BinaryEncoding binary_encoding;

  std::unordered_map<String, Ref<YAMLStyle>, StringHasher, StringEqual> child_styles;

  // Debug helper methods
  static String get_scalar_style_string(ScalarStyle p_style);
  static String get_quote_style_string(QuoteStyle p_style);
  static String get_container_form_string(ContainerForm p_style);
  static String get_flow_style_string(FlowStyle p_style);
  static String get_number_format_string(NumberFormat p_format);
  static String get_binary_encoding_string(BinaryEncoding p_encoding);
};

} // namespace godot

VARIANT_ENUM_CAST(YAMLStyle::ScalarStyle);
VARIANT_ENUM_CAST(YAMLStyle::QuoteStyle);
VARIANT_ENUM_CAST(YAMLStyle::ContainerForm);
VARIANT_ENUM_CAST(YAMLStyle::FlowStyle);
VARIANT_ENUM_CAST(YAMLStyle::NumberFormat);
VARIANT_ENUM_CAST(YAMLStyle::BinaryEncoding);

#endif // YAML_STYLE_H

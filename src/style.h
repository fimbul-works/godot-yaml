#ifndef YAML_STYLE_H
#define YAML_STYLE_H

#include "string_hash.h"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

#include <memory>
#include <unordered_map>

namespace godot {

class YAMLStyle : public RefCounted {
  GDCLASS(YAMLStyle, RefCounted);

  public:
  // Scalar style enums - simplified from original
  enum ScalarStyle {
    STYLE_ANY = 0, // Let emitter decide
    STYLE_PLAIN = 1, // Unquoted
    STYLE_QUOTED = 2, // Either single or double quotes
    STYLE_BLOCK = 3, // Block style (literal or folded)
  };

  enum BlockStyle {
    BLOCK_ANY = 0, // Let emitter decide
    BLOCK_LITERAL = 1, // | style
    BLOCK_FOLDED = 2, // > style
  };

  enum CollectionStyle {
    COLLECTION_ANY = 0, // Let emitter decide
    COLLECTION_BLOCK = 1, // [a, b, c]
    COLLECTION_FLOW = 2, // [a,b,c]
    MAP_BLOCK = 3, // key: value
    MAP_FLOW = 4, // {key: value}
  };

  enum NumberFormat {
    NUM_DECIMAL = 0,
    NUM_HEX = 1,
    NUM_OCTAL = 2,
    NUM_BINARY = 3,
    NUM_SCIENTIFIC = 4,
  };

  enum StringFormat {
    STRING_ANY = 0, // Let emitter decide
    STRING_PLAIN = 1, // No special formatting
    STRING_HEX = 2, // 0xRRGGBB[AA]
    STRING_HEX_STR = 3 // #RRGGBB[AA]
  };

  enum BinaryEncoding {
    BINARY_ANY = 0,
    BINARY_BASE64 = 1,
    BINARY_HEX = 2
  };

  // Node-specific style information - simplified
  ScalarStyle scalar_style = STYLE_ANY;
  BlockStyle block_style = BLOCK_ANY;
  CollectionStyle collection_style = COLLECTION_ANY;
  NumberFormat number_format = NUM_DECIMAL;
  StringFormat string_format = STRING_ANY;
  BinaryEncoding binary_encoding = BINARY_BASE64;

  Dictionary custom_settings; // For extensibility

  YAMLStyle();
  ~YAMLStyle() = default;

  // Child style management
  Ref<YAMLStyle> get_child(const String& key) const;
  void set_child(const String& key, const Ref<YAMLStyle>& style);
  bool has_child(const String& key) const;
  void clear_child(const String& key);
  void clear_children();

  // Add getter/setter methods
  ScalarStyle get_scalar_style() const { return scalar_style; }
  void set_scalar_style(ScalarStyle p_style) { scalar_style = p_style; }

  BlockStyle get_block_style() const { return block_style; }
  void set_block_style(BlockStyle p_style) { block_style = p_style; }

  CollectionStyle get_collection_style() const { return collection_style; }
  void set_collection_style(CollectionStyle p_style) { collection_style = p_style; }

  NumberFormat get_number_format() const { return number_format; }
  void set_number_format(NumberFormat p_format) { number_format = p_format; }

  StringFormat get_string_format() const { return string_format; }
  void set_string_format(StringFormat p_format) { string_format = p_format; }

  BinaryEncoding get_binary_encoding() const { return binary_encoding; }
  void set_binary_encoding(BinaryEncoding p_encoding) { binary_encoding = p_encoding; }

  // Conversion to/from Dictionary for GDScript interface
  Dictionary to_dict() const;
  Error from_dict(const Dictionary& dict);

  String _to_string() const;

  protected:
  static void _bind_methods();
  String _style_to_string(const String& indent = "") const;

  private:
  std::unordered_map<String, Ref<YAMLStyle>, StringHasher> child_styles;

  Dictionary _style_to_dict() const;
  Error _dict_to_style(const Dictionary& dict);
};

} // namespace godot

VARIANT_ENUM_CAST(YAMLStyle::ScalarStyle);
VARIANT_ENUM_CAST(YAMLStyle::BlockStyle);
VARIANT_ENUM_CAST(YAMLStyle::CollectionStyle);
VARIANT_ENUM_CAST(YAMLStyle::NumberFormat);
VARIANT_ENUM_CAST(YAMLStyle::StringFormat);
VARIANT_ENUM_CAST(YAMLStyle::BinaryEncoding);

#endif // YAML_STYLE_H

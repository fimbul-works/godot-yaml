// style.cpp
#include "style.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

YAMLStyle::YAMLStyle() = default;

void YAMLStyle::_bind_methods()
{
  // Bind enums
  BIND_ENUM_CONSTANT(STYLE_ANY);
  BIND_ENUM_CONSTANT(STYLE_PLAIN);
  BIND_ENUM_CONSTANT(STYLE_QUOTED);
  BIND_ENUM_CONSTANT(STYLE_BLOCK);

  BIND_ENUM_CONSTANT(BLOCK_ANY);
  BIND_ENUM_CONSTANT(BLOCK_LITERAL);
  BIND_ENUM_CONSTANT(BLOCK_FOLDED);

  BIND_ENUM_CONSTANT(COLLECTION_ANY);
  BIND_ENUM_CONSTANT(COLLECTION_BLOCK);
  BIND_ENUM_CONSTANT(COLLECTION_FLOW);
  BIND_ENUM_CONSTANT(MAP_BLOCK);
  BIND_ENUM_CONSTANT(MAP_FLOW);

  BIND_ENUM_CONSTANT(NUM_DECIMAL);
  BIND_ENUM_CONSTANT(NUM_HEX);
  BIND_ENUM_CONSTANT(NUM_OCTAL);
  BIND_ENUM_CONSTANT(NUM_BINARY);
  BIND_ENUM_CONSTANT(NUM_SCIENTIFIC);

  BIND_ENUM_CONSTANT(STRING_ANY);
  BIND_ENUM_CONSTANT(STRING_PLAIN);
  BIND_ENUM_CONSTANT(STRING_HEX);
  BIND_ENUM_CONSTANT(STRING_HEX_STR);

  BIND_ENUM_CONSTANT(BINARY_ANY);
  BIND_ENUM_CONSTANT(BINARY_BASE64);
  BIND_ENUM_CONSTANT(BINARY_HEX);

  // Bind properties
  ClassDB::bind_method(D_METHOD("get_scalar_style"), &YAMLStyle::get_scalar_style);
  ClassDB::bind_method(D_METHOD("set_scalar_style", "style"), &YAMLStyle::set_scalar_style);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "scalar_style", PROPERTY_HINT_ENUM, "Any,Plain,Quoted,Block"), "set_scalar_style", "get_scalar_style");

  ClassDB::bind_method(D_METHOD("get_block_style"), &YAMLStyle::get_block_style);
  ClassDB::bind_method(D_METHOD("set_block_style", "style"), &YAMLStyle::set_block_style);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "block_style", PROPERTY_HINT_ENUM, "Any,Literal,Folded"), "set_block_style", "get_block_style");

  ClassDB::bind_method(D_METHOD("get_collection_style"), &YAMLStyle::get_collection_style);
  ClassDB::bind_method(D_METHOD("set_collection_style", "style"), &YAMLStyle::set_collection_style);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "collection_style", PROPERTY_HINT_ENUM, "Any,Array,FlowArray,Block,FlowBlock"), "set_collection_style", "get_collection_style");

  ClassDB::bind_method(D_METHOD("get_number_format"), &YAMLStyle::get_number_format);
  ClassDB::bind_method(D_METHOD("set_number_format", "format"), &YAMLStyle::set_number_format);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "number_format", PROPERTY_HINT_ENUM, "Decimal,Hex,Octal,Binary,Scientific"), "set_number_format", "get_number_format");

  ClassDB::bind_method(D_METHOD("get_string_format"), &YAMLStyle::get_string_format);
  ClassDB::bind_method(D_METHOD("set_string_format", "format"), &YAMLStyle::set_string_format);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "string_format", PROPERTY_HINT_ENUM, "Any,Plain,Hex,HexString"), "set_string_format", "get_string_format");

  ClassDB::bind_method(D_METHOD("get_binary_encoding"), &YAMLStyle::get_binary_encoding);
  ClassDB::bind_method(D_METHOD("set_binary_encoding", "format"), &YAMLStyle::set_binary_encoding);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "binary_encoding", PROPERTY_HINT_ENUM, "Any,Base64,Hex"), "set_binary_encoding", "get_binary_encoding");

  // Child style management
  ClassDB::bind_method(D_METHOD("get_child", "key"), &YAMLStyle::get_child);
  ClassDB::bind_method(D_METHOD("set_child", "key", "style"), &YAMLStyle::set_child);
  ClassDB::bind_method(D_METHOD("has_child", "key"), &YAMLStyle::has_child);
  ClassDB::bind_method(D_METHOD("clear_child", "key"), &YAMLStyle::clear_child);
  ClassDB::bind_method(D_METHOD("clear_children"), &YAMLStyle::clear_children);

  // Dictionary conversion
  ClassDB::bind_method(D_METHOD("to_dict"), &YAMLStyle::to_dict);
  ClassDB::bind_method(D_METHOD("from_dict", "dict"), &YAMLStyle::from_dict);
}

Ref<YAMLStyle> YAMLStyle::get_child(const String& key) const
{
  auto it = child_styles.find(key);
  if (it != child_styles.end()) {
    return it->second;
  }
  return Ref<YAMLStyle>();
}

void YAMLStyle::set_child(const String& key, const Ref<YAMLStyle>& style)
{
  if (style.is_valid()) {
    child_styles[key] = style;
  } else {
    child_styles.erase(key);
  }
}

bool YAMLStyle::has_child(const String& key) const
{
  return child_styles.find(key) != child_styles.end();
}

void YAMLStyle::clear_child(const String& key)
{
  child_styles.erase(key);
}

void YAMLStyle::clear_children()
{
  child_styles.clear();
}

Dictionary YAMLStyle::to_dict() const
{
  Dictionary dict;

  // Core style properties
  dict["scalar_style"] = scalar_style;
  dict["block_style"] = block_style;
  dict["collection_style"] = collection_style;
  dict["number_format"] = number_format;
  dict["binary_encoding"] = binary_encoding;
  dict["custom_settings"] = custom_settings;

  // Child styles
  Dictionary children;
  for (const auto& pair : child_styles) {
    if (pair.second.is_valid()) {
      children[pair.first] = pair.second->to_dict();
    }
  }
  dict["children"] = children;

  return dict;
}

Error YAMLStyle::from_dict(const Dictionary& dict)
{
  // Core style properties
  scalar_style = (ScalarStyle)(int)dict.get("scalar_style", STYLE_ANY);
  block_style = (BlockStyle)(int)dict.get("block_style", BLOCK_ANY);
  collection_style = (CollectionStyle)(int)dict.get("collection_style", COLLECTION_ANY);
  number_format = (NumberFormat)(int)dict.get("number_format", NUM_DECIMAL);
  binary_encoding = (BinaryEncoding)(int)dict.get("binary_encoding", BINARY_BASE64);
  custom_settings = dict.get("custom_settings", Dictionary());

  // Child styles
  Dictionary children = dict.get("children", Dictionary());
  child_styles.clear();
  Array keys = children.keys();
  for (int i = 0; i < keys.size(); i++) {
    String key = keys[i];
    Dictionary child_dict = children[key];
    Ref<YAMLStyle> child;
    child.instantiate();
    if (child->from_dict(child_dict) == OK) {
      child_styles[key] = child;
    }
  }

  return OK;
}

String YAMLStyle::_to_string() const
{
  return _style_to_string();
}

String YAMLStyle::_style_to_string(const String& indent) const
{
  Array lines;

  // Add style properties
  if (scalar_style != STYLE_ANY) {
    lines.push_back(indent + String("Scalar: ") + String::num_int64(scalar_style));
  }
  if (block_style != BLOCK_ANY) {
    lines.push_back(indent + String("Block: ") + String::num_int64(block_style));
  }
  if (collection_style != COLLECTION_ANY) {
    lines.push_back(indent + String("Collection: ") + String::num_int64(collection_style));
  }
  if (number_format != NUM_DECIMAL) {
    lines.push_back(indent + String("Number Format: ") + String::num_int64(number_format));
  }
  if (string_format != STRING_ANY) {
    lines.push_back(indent + String("String Format: ") + String::num_int64(string_format));
  }
  if (binary_encoding != BINARY_ANY) {
    lines.push_back(indent + String("Binary Encoding: ") + String::num_int64(binary_encoding));
  }

  // Add custom settings if any
  if (!custom_settings.is_empty()) {
    String settings_str = "Custom Settings:";
    Array keys = custom_settings.keys();
    for (int i = 0; i < keys.size(); i++) {
      settings_str += "\n" + indent + "  " + String(keys[i]) + ": " + String(custom_settings[keys[i]]);
    }
    lines.push_back(indent + settings_str);
  }

  // Add child styles if any
  if (!child_styles.empty()) {
    lines.push_back(indent + String("Child Styles:"));
    for (const auto& pair : child_styles) {
      if (pair.second.is_valid()) {
        lines.push_back(indent + String("  ") + pair.first + ":");
        lines.push_back(pair.second->_style_to_string(indent + String("    ")));
      }
    }
  }

  // Join all lines
  if (lines.is_empty()) {
    return indent + String("(default style)");
  }

  return String("\n").join(lines);
}

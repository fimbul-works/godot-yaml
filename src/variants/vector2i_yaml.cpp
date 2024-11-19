#include "vector2i_yaml.h"
#include "../util_numeric.h"

using namespace godot;

Vector2iVariantConverter::Vector2iVariantConverter(YAML* yaml) :
        VariantConverter(yaml) { }

void Vector2iVariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  Vector2i vec = v.operator Vector2i();
  switch (format) {
    case Format::FLOW_MAP:
      emit_as_flow(node, vec);
      break;
    case Format::BLOCK_MAP:
      emit_as_block(node, vec);
      break;
    case Format::SEQUENCE:
      emit_as_sequence(node, vec);
      break;
    case Format::INLINE:
      emit_as_inline(node, vec);
      break;
  }
}

void Vector2iVariantConverter::emit_as_flow(ryml::NodeRef& node, const Vector2i& vec) const
{
  node |= ryml::MAP;
  node |= ryml::FLOW_SL;
  node["x"] << int_to_string(vec.x);
  node["y"] << int_to_string(vec.y);
}

void Vector2iVariantConverter::emit_as_block(ryml::NodeRef& node, const Vector2i& vec) const
{
  node |= ryml::MAP;
  node["x"] << int_to_string(vec.x);
  node["y"] << int_to_string(vec.y);
}

void Vector2iVariantConverter::emit_as_sequence(ryml::NodeRef& node, const Vector2i& vec) const
{
  node |= ryml::SEQ;
  node |= ryml::FLOW_SL;
  node.append_child() << int_to_string(vec.x);
  node.append_child() << int_to_string(vec.y);
}

void Vector2iVariantConverter::emit_as_inline(ryml::NodeRef& node, const Vector2i& vec) const
{
  // Create a static buffer large enough for the format "(x, y)"
  static thread_local char buf[256]; // Large enough for two ints plus formatting
  size_t len = ryml::format(buf, "({}, {})", int_to_string(vec.x), int_to_string(vec.y));
  node << ryml::csubstr(buf, len);
}

Variant Vector2iVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  try {
    if (node.is_map()) {
      return decode_map(node);
    } else if (node.is_seq()) {
      return decode_sequence(node);
    } else if (node.has_val()) {
      return decode_inline(node.val());
    }
    throw YAMLException::create_invalid_format("Vector2i");
  } catch (const YAMLException&) {
    throw; // Re-throw YAML exceptions
  } catch (const std::exception& e) {
    throw YAMLException(String("Failed to decode Vector2i: ") + e.what());
  }
}

Variant Vector2iVariantConverter::decode_map(const ryml::ConstNodeRef& node) const
{
  if (!node.has_child("x") || !node.has_child("y")) {
    throw YAMLException::create_missing_field("Vector2i", "x, y");
  }

  int32_t x = string_to_int<int32_t>(node["x"].val());
  int32_t y = string_to_int<int32_t>(node["y"].val());
  return Vector2i(x, y);
}

Variant Vector2iVariantConverter::decode_sequence(const ryml::ConstNodeRef& node) const
{
  if (node.num_children() != 2) {
    throw YAMLException::create_invalid_sequence_length("Vector2i", 2);
  }

  int32_t x = string_to_int<int32_t>(node[0].val());
  int32_t y = string_to_int<int32_t>(node[1].val());
  return Vector2i(x, y);
}

Variant Vector2iVariantConverter::decode_inline(const ryml::csubstr& val) const
{
  // Parse "(x, y)" format
  std::string str(val.str, val.len);
  if (str.length() < 5 || str[0] != '(' || str[str.length() - 1] != ')') { // Minimum "(0,0)"
    throw YAMLException("Invalid inline Vector2i format - expected (x, y)");
  }

  // Remove parentheses and split at comma
  str = str.substr(1, str.length() - 2);
  size_t comma_pos = str.find(',');
  if (comma_pos == std::string::npos) {
    throw YAMLException("Invalid inline Vector2i format - missing comma");
  }

  try {
    int32_t x = string_to_int<int32_t>(str.substr(0, comma_pos));
    int32_t y = string_to_int<int32_t>(str.substr(comma_pos + 1));
    return Vector2i(x, y);
  } catch (const std::exception& e) {
    throw YAMLException(String("Failed to parse Vector2i components: ") + e.what());
  }
}

bool Vector2iVariantConverter::set_format(const String& format_str)
{
  if (format_str == "flow") {
    format = Format::FLOW_MAP;
  } else if (format_str == "block") {
    format = Format::BLOCK_MAP;
  } else if (format_str == "sequence") {
    format = Format::SEQUENCE;
  } else if (format_str == "inline") {
    format = Format::INLINE;
  } else {
    return false;
  }
  return true;
}

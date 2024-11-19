#include "vector2_yaml.h"
#include "../util_numeric.h"

using namespace godot;

Vector2VariantConverter::Vector2VariantConverter(YAML* yaml) :
        VariantConverter(yaml) { }

void Vector2VariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  Vector2 vec = v.operator Vector2();
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

void Vector2VariantConverter::emit_as_flow(ryml::NodeRef& node, const Vector2& vec) const
{
  node |= ryml::MAP;
  node |= ryml::FLOW_SL;
  node["x"] << float_to_string(vec.x);
  node["y"] << float_to_string(vec.y);
}

void Vector2VariantConverter::emit_as_block(ryml::NodeRef& node, const Vector2& vec) const
{
  node |= ryml::MAP;
  node["x"] << float_to_string(vec.x);
  node["y"] << float_to_string(vec.y);
}

void Vector2VariantConverter::emit_as_sequence(ryml::NodeRef& node, const Vector2& vec) const
{
  node |= ryml::SEQ;
  node |= ryml::FLOW_SL;
  node.append_child() << float_to_string(vec.x);
  node.append_child() << float_to_string(vec.y);
}

void Vector2VariantConverter::emit_as_inline(ryml::NodeRef& node, const Vector2& vec) const
{
  // Create a static buffer large enough for the format "(x, y)"
  static thread_local char buf[256]; // Large enough for two doubles plus formatting
  size_t len = ryml::format(buf, "({}, {})", float_to_string(vec.x), float_to_string(vec.y));
  node << ryml::csubstr(buf, len);
}

Variant Vector2VariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  try {
    if (node.is_map()) {
      return decode_map(node);
    } else if (node.is_seq()) {
      return decode_sequence(node);
    } else if (node.has_val()) {
      return decode_inline(node.val());
    }
    throw YAMLException::create_invalid_format("Vector2");
  } catch (const YAMLException&) {
    throw; // Re-throw YAML exceptions
  } catch (const std::exception& e) {
    throw YAMLException(String("Failed to decode Vector2: ") + e.what());
  }
}

Variant Vector2VariantConverter::decode_map(const ryml::ConstNodeRef& node) const
{
  if (!node.has_child("x") || !node.has_child("y")) {
    throw YAMLException::create_missing_field("Vector2", "x, y");
  }

  real_t x = string_to_float<real_t>(node["x"].val());
  real_t y = string_to_float<real_t>(node["y"].val());
  return Vector2(x, y);
}

Variant Vector2VariantConverter::decode_sequence(const ryml::ConstNodeRef& node) const
{
  if (node.num_children() != 2) {
    throw YAMLException::create_invalid_sequence_length("Vector2", 2);
  }

  real_t x = string_to_float<real_t>(node[0].val());
  real_t y = string_to_float<real_t>(node[1].val());
  return Vector2(x, y);
}

Variant Vector2VariantConverter::decode_inline(const ryml::csubstr& val) const
{
  // Parse "(x, y)" format
  std::string str(val.str, val.len);
  if (str.length() < 5 || str[0] != '(' || str[str.length() - 1] != ')') { // Minimum "(0,0)"
    throw YAMLException("Invalid inline Vector2 format - expected (x, y)");
  }

  // Remove parentheses and split at comma
  str = str.substr(1, str.length() - 2);
  size_t comma_pos = str.find(',');
  if (comma_pos == std::string::npos) {
    throw YAMLException("Invalid inline Vector2 format - missing comma");
  }

  try {
    real_t x = string_to_float<real_t>(str.substr(0, comma_pos));
    real_t y = string_to_float<real_t>(str.substr(comma_pos + 1));
    return Vector2(x, y);
  } catch (const std::exception& e) {
    throw YAMLException(String("Failed to parse Vector2 components: ") + e.what());
  }
}

bool Vector2VariantConverter::set_format(const String& format_str)
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

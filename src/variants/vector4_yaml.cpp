#include "vector4_yaml.h"
#include "../util_numeric.h"

using namespace godot;

Vector4VariantConverter::Vector4VariantConverter(YAML* yaml) :
        VariantConverter(yaml) { }

void Vector4VariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  Vector4 vec = v.operator Vector4();
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

void Vector4VariantConverter::emit_as_flow(ryml::NodeRef& node, const Vector4& vec) const
{
  node |= ryml::MAP;
  node |= ryml::FLOW_SL;
  node["x"] << float_to_string(vec.x);
  node["y"] << float_to_string(vec.y);
  node["z"] << float_to_string(vec.z);
  node["w"] << float_to_string(vec.w);
}

void Vector4VariantConverter::emit_as_block(ryml::NodeRef& node, const Vector4& vec) const
{
  node |= ryml::MAP;
  node["x"] << float_to_string(vec.x);
  node["y"] << float_to_string(vec.y);
  node["z"] << float_to_string(vec.z);
  node["w"] << float_to_string(vec.w);
}

void Vector4VariantConverter::emit_as_sequence(ryml::NodeRef& node, const Vector4& vec) const
{
  node |= ryml::SEQ;
  node |= ryml::FLOW_SL;
  node.append_child() << float_to_string(vec.x);
  node.append_child() << float_to_string(vec.y);
  node.append_child() << float_to_string(vec.z);
  node.append_child() << float_to_string(vec.w);
}

void Vector4VariantConverter::emit_as_inline(ryml::NodeRef& node, const Vector4& vec) const
{
  static thread_local char buf[256]; // Generous buffer size
  size_t len = ryml::format(buf, "({}, {}, {}, {})",
          float_to_string(vec.x),
          float_to_string(vec.y),
          float_to_string(vec.z),
          float_to_string(vec.w));
  node << ryml::csubstr(buf, len);
}

Variant Vector4VariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  try {
    if (node.is_map()) {
      return decode_map(node);
    } else if (node.is_seq()) {
      return decode_sequence(node);
    } else if (node.has_val()) {
      return decode_inline(node.val());
    }
    throw YAMLException::create_invalid_format("Vector4");
  } catch (const YAMLException&) {
    throw; // Re-throw YAML exceptions
  } catch (const std::exception& e) {
    throw YAMLException(String("Failed to decode Vector4: ") + e.what());
  }
}

Variant Vector4VariantConverter::decode_map(const ryml::ConstNodeRef& node) const
{
  if (!node.has_child("x") || !node.has_child("y") || !node.has_child("z") || !node.has_child("w")) {
    throw YAMLException::create_missing_field("Vector4", "x, y, z, w");
  }

  real_t x = string_to_float<real_t>(node["x"].val());
  real_t y = string_to_float<real_t>(node["y"].val());
  real_t z = string_to_float<real_t>(node["z"].val());
  real_t w = string_to_float<real_t>(node["w"].val());
  return Vector4(x, y, z, w);
}

Variant Vector4VariantConverter::decode_sequence(const ryml::ConstNodeRef& node) const
{
  if (node.num_children() != 4) {
    throw YAMLException::create_invalid_sequence_length("Vector4", 4);
  }

  real_t x = string_to_float<real_t>(node[0].val());
  real_t y = string_to_float<real_t>(node[1].val());
  real_t z = string_to_float<real_t>(node[2].val());
  real_t w = string_to_float<real_t>(node[3].val());
  return Vector4(x, y, z, w);
}

Variant Vector4VariantConverter::decode_inline(const ryml::csubstr& val) const
{
  // Parse "(x, y, z, w)" format
  std::string str(val.str, val.len);
  if (str.length() < 9 || str[0] != '(' || str[str.length() - 1] != ')') { // Minimum "(0,0,0,0)"
    throw YAMLException("Invalid inline Vector4 format - expected (x, y, z, w)");
  }

  // Remove parentheses and find commas
  str = str.substr(1, str.length() - 2);
  size_t first_comma = str.find(',');
  if (first_comma == std::string::npos) {
    throw YAMLException("Invalid inline Vector4 format - missing first comma");
  }

  size_t second_comma = str.find(',', first_comma + 1);
  if (second_comma == std::string::npos) {
    throw YAMLException("Invalid inline Vector4 format - missing second comma");
  }

  size_t third_comma = str.find(',', second_comma + 1);
  if (third_comma == std::string::npos) {
    throw YAMLException("Invalid inline Vector4 format - missing third comma");
  }

  try {
    real_t x = string_to_float<real_t>(str.substr(0, first_comma));
    real_t y = string_to_float<real_t>(str.substr(first_comma + 1, second_comma - first_comma - 1));
    real_t z = string_to_float<real_t>(str.substr(second_comma + 1, third_comma - second_comma - 1));
    real_t w = string_to_float<real_t>(str.substr(third_comma + 1));
    return Vector4(x, y, z, w);
  } catch (const std::exception& e) {
    throw YAMLException(String("Failed to parse Vector4 components: ") + e.what());
  }
}

bool Vector4VariantConverter::set_format(const String& format_str)
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

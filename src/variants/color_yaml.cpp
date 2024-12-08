#include "color_yaml.h"
#include "../exception.h"
#include "../util_numeric.h"

using namespace godot;

void ColorVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const Ref<YAMLStyle>& style) const
{
  const Color color = v.operator Color();
  const bool has_alpha = color.a < 1.0f;

  if (!style.is_valid() || style->collection_style == YAMLStyle::COLLECTION_ANY) {
    // Default to hex string if no style specified
    emit_as_hex(node, color, has_alpha, "#");
    return;
  }

  if (style->scalar_style == YAMLStyle::STYLE_PLAIN) {
    switch (style->string_format) {
      case YAMLStyle::STRING_HEX:
        emit_as_hex(node, color, has_alpha, "0x");
        break;
      case YAMLStyle::STRING_HEX_STR:
        emit_as_hex(node, color, has_alpha, "#");
        break;
      default:
        // Fall back to structured format if no string format specified
        if (style->collection_style == YAMLStyle::COLLECTION_FLOW || style->collection_style == YAMLStyle::COLLECTION_BLOCK) {
          emit_as_sequence(node, color, style);
        } else {
          emit_as_map(node, color, style);
        }
        break;
    }
  } else {
    // Use collection format
    if (style->collection_style == YAMLStyle::COLLECTION_FLOW || style->collection_style == YAMLStyle::COLLECTION_BLOCK) {
      emit_as_sequence(node, color, style);
    } else {
      emit_as_map(node, color, style);
    }
  }
}

void ColorVariantConverter::emit_as_hex(ryml::NodeRef& node, const Color& color, bool with_alpha, const char* prefix) const
{
  node << color_to_hex(color, with_alpha, prefix).c_str();
}

void ColorVariantConverter::emit_as_map(ryml::NodeRef& node, const Color& color, const Ref<YAMLStyle>& style) const
{
  // Map styles
  node |= ryml::MAP;
  if (!style.is_valid() || style->collection_style == YAMLStyle::MAP_FLOW) {
    node |= ryml::FLOW_SL;
  }

  node["r"] << float_to_string(color.r);
  node["g"] << float_to_string(color.g);
  node["b"] << float_to_string(color.b);
  if (color.a < 1.0f) {
    node["a"] << float_to_string(color.a);
  }
}

void ColorVariantConverter::emit_as_sequence(ryml::NodeRef& node, const Color& color, const Ref<YAMLStyle>& style) const
{
  node |= ryml::SEQ;
  if (!style.is_valid() || style->collection_style == YAMLStyle::COLLECTION_FLOW) {
    node |= ryml::FLOW_SL;
  }

  node.append_child() << float_to_string(color.r);
  node.append_child() << float_to_string(color.g);
  node.append_child() << float_to_string(color.b);
  if (color.a < 1.0f) {
    node.append_child() << float_to_string(color.a);
  }
}

Variant ColorVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  try {
    if (node.has_val() && !node.val_is_null()) {
      if (node.val().begins_with("0x") || node.val().begins_with("#")) {
        return decode_hex(node.val());
      }
    }

    if (node.is_map()) {
      return decode_map(node);
    }

    if (node.is_seq()) {
      return decode_sequence(node);
    }

    throw YAMLException::create_invalid_format("Color");
  } catch (const YAMLException&) {
    throw;
  } catch (const std::exception& e) {
    throw YAMLException::create_decode_error("Color", e.what());
  }
}

Color ColorVariantConverter::parse_hex_components(const std::string& hex_str, int offset, size_t expected_length) const
{
  if (hex_str.length() != expected_length && hex_str.length() != expected_length + HEX_ALPHA_EXTRA) {
    throw YAMLException("Invalid hex color length");
  }

  try {
    int r = std::stoi(hex_str.substr(offset, 2), nullptr, 16);
    int g = std::stoi(hex_str.substr(offset + 2, 2), nullptr, 16);
    int b = std::stoi(hex_str.substr(offset + 4, 2), nullptr, 16);
    int a = hex_str.length() == expected_length + HEX_ALPHA_EXTRA ? std::stoi(hex_str.substr(offset + 6, 2), nullptr, 16) : 255;

    return Color(r / COLOR_COMPONENT_MAX,
            g / COLOR_COMPONENT_MAX,
            b / COLOR_COMPONENT_MAX,
            a / COLOR_COMPONENT_MAX);
  } catch (const std::exception& e) {
    throw YAMLException("Invalid hex color component");
  }
}

Variant ColorVariantConverter::decode_hex(const ryml::csubstr& val) const
{
  std::string hex_str(val.str, val.len);
  try {
    if (hex_str[0] == '#') {
      return parse_hex_components(hex_str, 1, HEX_STRING_LENGTH);
    } else if (hex_str.compare(0, 2, "0x") == 0) {
      return parse_hex_components(hex_str, 2, HEX_NUMBER_LENGTH);
    }
    throw YAMLException("Invalid hex color format");
  } catch (const YAMLException&) {
    throw;
  } catch (const std::exception& e) {
    throw YAMLException(String("Failed to parse hex color: ") + e.what());
  }
}

Variant ColorVariantConverter::decode_map(const ryml::ConstNodeRef& node) const
{
  if (!node.has_child("r") || !node.has_child("g") || !node.has_child("b")) {
    throw YAMLException::create_missing_field("Color", "r, g, b");
  }

  real_t r = string_to_float<real_t>(node["r"].val());
  real_t g = string_to_float<real_t>(node["g"].val());
  real_t b = string_to_float<real_t>(node["b"].val());
  real_t a = node.has_child("a") ? string_to_float<real_t>(node["a"].val()) : 1.0f;

  return Color(r, g, b, a);
}

Variant ColorVariantConverter::decode_sequence(const ryml::ConstNodeRef& node) const
{
  const size_t size = node.num_children();
  if (size != 3 && size != 4) {
    throw YAMLException("Color sequence must have 3 or 4 elements (RGB[A])");
  }

  real_t r = string_to_float<real_t>(node[0].val());
  real_t g = string_to_float<real_t>(node[1].val());
  real_t b = string_to_float<real_t>(node[2].val());
  real_t a = size == 4 ? string_to_float<real_t>(node[3].val()) : 1.0f;

  return Color(r, g, b, a);
}

std::string ColorVariantConverter::color_to_hex(const Color& color, bool with_alpha, const char* prefix) const
{
  char buffer[11]; // Enough for prefix + RRGGBBAA + null
  if (with_alpha) {
    snprintf(buffer, sizeof(buffer), "%s%02X%02X%02X%02X",
            prefix,
            static_cast<int>(color.r * COLOR_COMPONENT_MAX),
            static_cast<int>(color.g * COLOR_COMPONENT_MAX),
            static_cast<int>(color.b * COLOR_COMPONENT_MAX),
            static_cast<int>(color.a * COLOR_COMPONENT_MAX));
  } else {
    snprintf(buffer, sizeof(buffer), "%s%02X%02X%02X",
            prefix,
            static_cast<int>(color.r * COLOR_COMPONENT_MAX),
            static_cast<int>(color.g * COLOR_COMPONENT_MAX),
            static_cast<int>(color.b * COLOR_COMPONENT_MAX));
  }
  return std::string(buffer);
}

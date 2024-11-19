#include "color_yaml.h"
#include "../util_numeric.h"
#include "../yaml_exception.h"

using namespace godot;

ColorVariantConverter::ColorVariantConverter(YAML* yaml) :
        VariantConverter(yaml) { }

void ColorVariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  Color color = v.operator Color();
  switch (format) {
    case Format::HEX:
      node << color_to_hex(color, "0x");
      break;
    case Format::HEX_STRING:
      node << color_to_hex(color, "#");
      break;
    case Format::FLOW_MAP:
      emit_as_flow(node, color);
      break;
    case Format::SEQUENCE:
      emit_as_sequence(node, color);
      break;
  }
}

Variant ColorVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  try {
    if (node.has_val() && !node.val_is_null()) {
      // Handle hex format
      return hex_to_color(std::string(node.val().str, node.val().len));
    } else if (node.is_map()) {
      if (!node.has_child("r") || !node.has_child("g") || !node.has_child("b")) {
        throw YAMLException::create_missing_field("Color", "r, g, b");
      }
      float r = string_to_float<float>(node["r"].val());
      float g = string_to_float<float>(node["g"].val());
      float b = string_to_float<float>(node["b"].val());
      float a = node.has_child("a") ? string_to_float<float>(node["a"].val()) : 1.0f;
      return Color(r, g, b, a);
    } else if (node.is_seq()) {
      if (node.num_children() < 3 || node.num_children() > 4) {
        throw YAMLException("Color sequence must have 3 or 4 elements (RGB[A])");
      }
      float r = string_to_float<float>(node[0].val());
      float g = string_to_float<float>(node[1].val());
      float b = string_to_float<float>(node[2].val());
      float a = node.num_children() == 4 ? string_to_float<float>(node[3].val()) : 1.0f;
      return Color(r, g, b, a);
    }
    throw YAMLException::create_invalid_format("Color");
  } catch (const YAMLException&) {
    throw; // Re-throw YAML exceptions
  } catch (const std::exception& e) {
    throw YAMLException(String("Failed to parse Color: ") + e.what());
  }
}

bool ColorVariantConverter::set_format(const String& format_str)
{
  if (format_str == "hex") {
    format = Format::HEX;
  } else if (format_str == "hex_string") {
    format = Format::HEX_STRING;
  } else if (format_str == "flow") {
    format = Format::FLOW_MAP;
  } else if (format_str == "sequence") {
    format = Format::SEQUENCE;
  } else {
    return false;
  }
  return true;
}

Color ColorVariantConverter::hex_to_color(const std::string& hex) const
{
  try {
    if (hex[0] == '#' && (hex.length() == 7 || hex.length() == 9)) {
      // #RRGGBB[AA] format
      int r = std::stoi(hex.substr(1, 2), nullptr, 16);
      int g = std::stoi(hex.substr(3, 2), nullptr, 16);
      int b = std::stoi(hex.substr(5, 2), nullptr, 16);
      int a = hex.length() == 9 ? std::stoi(hex.substr(7, 2), nullptr, 16) : 255;
      return Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
    } else if (hex.compare(0, 2, "0x") == 0 && (hex.length() == 8 || hex.length() == 10)) {
      // 0xRRGGBB[AA] format
      int r = std::stoi(hex.substr(2, 2), nullptr, 16);
      int g = std::stoi(hex.substr(4, 2), nullptr, 16);
      int b = std::stoi(hex.substr(6, 2), nullptr, 16);
      int a = hex.length() == 10 ? std::stoi(hex.substr(8, 2), nullptr, 16) : 255;
      return Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
    }
    throw YAMLException("Invalid color hex format");
  } catch (const std::exception& e) {
    throw YAMLException(String("Failed to parse color hex value: ") + hex.c_str());
  }
}

std::string ColorVariantConverter::color_to_hex(const Color& color, const char* prefix) const
{
  char buffer[11]; // Enough for prefix + RRGGBBAA + null
  if (color.a < 1.0f) {
    snprintf(buffer, sizeof(buffer), "%s%02X%02X%02X%02X",
            prefix,
            static_cast<int>(color.r * 255),
            static_cast<int>(color.g * 255),
            static_cast<int>(color.b * 255),
            static_cast<int>(color.a * 255));
  } else {
    snprintf(buffer, sizeof(buffer), "%s%02X%02X%02X",
            prefix,
            static_cast<int>(color.r * 255),
            static_cast<int>(color.g * 255),
            static_cast<int>(color.b * 255));
  }
  return std::string(buffer);
}

void ColorVariantConverter::emit_as_flow(ryml::NodeRef& node, const Color& color) const
{
  node |= ryml::MAP;
  node |= ryml::FLOW_SL;
  node["r"] << float_to_string(color.r);
  node["g"] << float_to_string(color.g);
  node["b"] << float_to_string(color.b);
  if (color.a < 1.0f) {
    node["a"] << float_to_string(color.a);
  }
}

void ColorVariantConverter::emit_as_sequence(ryml::NodeRef& node, const Color& color) const
{
  node |= ryml::SEQ;
  node |= ryml::FLOW_SL;
  node.append_child() << float_to_string(color.r);
  node.append_child() << float_to_string(color.g);
  node.append_child() << float_to_string(color.b);
  if (color.a < 1.0f) {
    node.append_child() << float_to_string(color.a);
  }
}

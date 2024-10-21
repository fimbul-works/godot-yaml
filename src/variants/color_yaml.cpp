#include "color_yaml.h"
#include "util_numeric.h"
#include "yaml.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

ColorYAMLEncoder::ColorYAMLEncoder(YAML* yaml) :
        YAMLEncoder(yaml) { }

void ColorYAMLEncoder::encode(ryml::NodeRef& node, const Variant& v) const
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

Variant ColorYAMLEncoder::decode(const ryml::ConstNodeRef& node) const
{
  if (node.has_val() && !node.val_is_null()) {
    std::string hex_str(node.val().str, node.val().len);
    return hex_to_color(hex_str);
  } else if (node.is_map()) {
    float r = string_to_float<float>(node["r"].val());
    float g = string_to_float<float>(node["g"].val());
    float b = string_to_float<float>(node["b"].val());
    float a = node.has_child("a") ? string_to_float<float>(node["a"].val()) : 1.0f;
    return Color(r, g, b, a);
  } else if (node.is_seq() && (node.num_children() == 3 || node.num_children() == 4)) {
    float r = string_to_float<float>(node[0].val());
    float g = string_to_float<float>(node[1].val());
    float b = string_to_float<float>(node[2].val());
    float a = node.num_children() == 4 ? string_to_float<float>(node[3].val()) : 1.0f;
    return Color(r, g, b, a);
  }
  throw YAMLException("invalid Color format - " + String::utf8(node.val().str, node.val().len));
}

bool ColorYAMLEncoder::set_format(const String& format_str)
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
    UtilityFunctions::printerr("YAML error: invalid format for Color - ", format_str);
    return false;
  }
  return true;
}

Color ColorYAMLEncoder::hex_to_color(const std::string& hex) const
{
  if (hex.at(0) == '#' && (hex.length() == 7 || hex.length() == 9)) {
    // #RRGGBB or #RRGGBBAA
    int r = std::stoi(hex.substr(1, 2), nullptr, 16);
    int g = std::stoi(hex.substr(3, 2), nullptr, 16);
    int b = std::stoi(hex.substr(5, 2), nullptr, 16);
    int a = hex.length() == 9 ? std::stoi(hex.substr(7, 2), nullptr, 16) : 255;
    return Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
  } else if (hex._Starts_with("0x") && (hex.length() == 8 || hex.length() == 10)) {
    // 0xRRGGBB or 0xRRGGBBAA
    int r = std::stoi(hex.substr(2, 2), nullptr, 16);
    int g = std::stoi(hex.substr(4, 2), nullptr, 16);
    int b = std::stoi(hex.substr(6, 2), nullptr, 16);
    int a = hex.length() == 10 ? std::stoi(hex.substr(8, 2), nullptr, 16) : 255;
    return Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
  }
  throw YAMLException("invalid Color format - " + hex);
}

std::string ColorYAMLEncoder::color_to_hex(const Color& color, const char* prefix) const
{
  char buffer[11]; // prefix(2) + RRGGBBAA(8) + null terminator(1)
  if (color.a < 1.0f) {
    int length = snprintf(buffer, sizeof(buffer), "%s%02X%02X%02X%02X", prefix, int(color.r * 255), int(color.g * 255), int(color.b * 255), int(color.a * 255));
    return std::string(buffer, length);
  } else {
    int length = snprintf(buffer, sizeof(buffer), "%s%02X%02X%02X", prefix, int(color.r * 255), int(color.g * 255), int(color.b * 255));
    return std::string(buffer, length);
  }
}

void ColorYAMLEncoder::emit_as_flow(ryml::NodeRef& node, const Color& color) const
{
  node |= ryml::MAP;
  node |= ryml::FLOW_SL;
  node["r"] << float_to_string(color.r);
  node["g"] << float_to_string(color.g);
  node["b"] << float_to_string(color.b);
  if (color.a != 1.0f) {
    node["a"] << float_to_string(color.a);
  }
}

void ColorYAMLEncoder::emit_as_sequence(ryml::NodeRef& node, const Color& color) const
{
  node |= ryml::SEQ;
  node |= ryml::FLOW_SL;
  node.append_child() << float_to_string(color.r);
  node.append_child() << float_to_string(color.g);
  node.append_child() << float_to_string(color.b);
  if (color.a != 1.0f) {
    node.append_child() << float_to_string(color.a);
  }
}

#include "color_yaml.h"
#include "util_numeric.h"
#include <godot_cpp/variant/utility_functions.hpp>
#include <iomanip>
#include <sstream>

namespace godot {
namespace variants {

  Color hex_to_color(const std::string& hex)
  {
    // #RRGGBB or #RRGGBBAA
    if (hex.at(0) != '#' || (hex.length() != 7 && hex.length() != 9)) {
      UtilityFunctions::printerr("Invalid Color format: ", hex.c_str());
      return Color();
    }

    int r = std::stoi(hex.substr(1, 2), nullptr, 16);
    int g = std::stoi(hex.substr(3, 2), nullptr, 16);
    int b = std::stoi(hex.substr(5, 2), nullptr, 16);
    int a = hex.length() == 9 ? std::stoi(hex.substr(7, 2), nullptr, 16) : 255;

    return Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
  }

  std::string color_to_hex(const Color& color)
  {
    std::stringstream ss;

    ss << "#"
       << std::setfill('0') << std::setw(2) << std::hex << int(color.r * 255)
       << std::setfill('0') << std::setw(2) << std::hex << int(color.g * 255)
       << std::setfill('0') << std::setw(2) << std::hex << int(color.b * 255);

    if (color.a < 1.0f) {
      ss << std::setfill('0') << std::setw(2) << std::hex << int(color.a * 255);
    }

    return ss.str();
  }

  template <>
  Color parse<Color>(const ryml::ConstNodeRef& node)
  {
    if (node.is_keyval()) {
      std::string hex_str = node_value_to_string(node);
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
      float a = node.num_children() > 3 ? string_to_float<float>(node[3].val()) : 1.0f;
      return Color(r, g, b, a);
    } else {
      UtilityFunctions::printerr("Invalid Color format: ", node_value_to_string(node).c_str());
      return Color();
    }
  }

  void emit_color_as_hex(ryml::NodeRef& node, const Color& color)
  {
    node << color_to_hex(color);
  }

  void emit_color_as_flow(ryml::NodeRef& node, const Color& color)
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

  template <>
  void emit<Color>(ryml::NodeRef& node, const Color& color)
  {
    emit_color_as_flow(node, color);
  }

} // namespace variants
} // namespace godot

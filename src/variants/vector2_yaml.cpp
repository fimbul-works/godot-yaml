#include "vector2_yaml.h"
#include "util_numeric.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include <iomanip>
#include <sstream>

using namespace godot;

const char* Vector2YAMLEncoder::get_tag() const
{
  return "Vector2";
}

void Vector2YAMLEncoder::encode(ryml::NodeRef& node, const Variant& v) const
{
  Vector2 vec = v.operator Vector2();
  switch (format) {
    case Format::FLOW_MAP:
      emit_as_flow(node, vec);
      break;
    case Format::SEQUENCE:
      emit_as_sequence(node, vec);
      break;
  }
}

Variant Vector2YAMLEncoder::decode(const ryml::ConstNodeRef& node) const
{
  if (node.is_map()) {
    real_t x = string_to_float<real_t>(node["x"].val());
    real_t y = string_to_float<real_t>(node["y"].val());
    return Vector2(x, y);
  } else if (node.is_seq() && node.num_children() == 2) {
    real_t x = string_to_float<real_t>(node[0].val());
    real_t y = string_to_float<real_t>(node[1].val());
    return Vector2(x, y);
  }
  throw YAMLException("invalid Vector2 format - " + String::utf8(node.val().str, node.val().len));
}

bool Vector2YAMLEncoder::set_format(const String& format_str)
{
  if (format_str == "flow") {
    format = Format::FLOW_MAP;
  } else if (format_str == "sequence") {
    format = Format::SEQUENCE;
  } else {
    UtilityFunctions::printerr("YAML error: invalid format for Vector2 - ", format_str);
    return false;
  }
  return true;
}

void Vector2YAMLEncoder::emit_as_flow(ryml::NodeRef& node, const Vector2& vec) const
{
  node |= ryml::MAP;
  node |= ryml::FLOW_SL;
  node["x"] << float_to_string(vec.x);
  node["y"] << float_to_string(vec.y);
}

void Vector2YAMLEncoder::emit_as_sequence(ryml::NodeRef& node, const Vector2& vec) const
{
  node |= ryml::SEQ;
  node |= ryml::FLOW_SL;
  node.append_child() << float_to_string(vec.x);
  node.append_child() << float_to_string(vec.y);
}

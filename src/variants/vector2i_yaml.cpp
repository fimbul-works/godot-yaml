#include "vector2i_yaml.h"
#include "util_numeric.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

Vector2iYAMLEncoder::Vector2iYAMLEncoder(YAML* yaml) :
        YAMLEncoder(yaml) { }

void Vector2iYAMLEncoder::encode(ryml::NodeRef& node, const Variant& v) const
{
  Vector2i vec = v.operator Vector2i();
  switch (format) {
    case Format::FLOW_MAP:
      emit_as_flow(node, vec);
      break;
    case Format::SEQUENCE:
      emit_as_sequence(node, vec);
      break;
  }
}

Variant Vector2iYAMLEncoder::decode(const ryml::ConstNodeRef& node) const
{
  if (node.is_map()) {
    int32_t x = string_to_int<int32_t>(node["x"].val());
    int32_t y = string_to_int<int32_t>(node["y"].val());
    return Vector2i(x, y);
  } else if (node.is_seq() && node.num_children() == 2) {
    int32_t x = string_to_int<int32_t>(node[0].val());
    int32_t y = string_to_int<int32_t>(node[1].val());
    return Vector2i(x, y);
  }
  throw YAMLException("invalid Vector2i format - " + String::utf8(node.val().str, node.val().len));
}

bool Vector2iYAMLEncoder::set_format(const String& format_str)
{
  if (format_str == "flow") {
    format = Format::FLOW_MAP;
  } else if (format_str == "sequence") {
    format = Format::SEQUENCE;
  } else {
    UtilityFunctions::printerr("YAML error: invalid format for Vector2i - ", format_str);
    return false;
  }
  return true;
}

void Vector2iYAMLEncoder::emit_as_flow(ryml::NodeRef& node, const Vector2i& vec) const
{
  node |= ryml::MAP;
  node |= ryml::FLOW_SL;
  node["x"] << int_to_string(vec.x);
  node["y"] << int_to_string(vec.y);
}

void Vector2iYAMLEncoder::emit_as_sequence(ryml::NodeRef& node, const Vector2i& vec) const
{
  node |= ryml::SEQ;
  node |= ryml::FLOW_SL;
  node.append_child() << int_to_string(vec.x);
  node.append_child() << int_to_string(vec.y);
}

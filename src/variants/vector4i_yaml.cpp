#include "vector4i_yaml.h"
#include "util_numeric.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include <iomanip>
#include <sstream>

using namespace godot;

const char* Vector4iYAMLEncoder::get_tag() const
{
  return "!!Vector4i";
}

void Vector4iYAMLEncoder::encode(ryml::NodeRef& node, const Variant& v) const
{
  Vector4i vec = v.operator Vector4i();
  switch (format) {
    case Format::FLOW_MAP:
      emit_as_flow(node, vec);
      break;
    case Format::SEQUENCE:
      emit_as_sequence(node, vec);
      break;
  }
}

Variant Vector4iYAMLEncoder::decode(const ryml::ConstNodeRef& node) const
{
  if (node.is_map()) {
    int32_t x = string_to_int<int32_t>(node["x"].val());
    int32_t y = string_to_int<int32_t>(node["y"].val());
    int32_t z = string_to_int<int32_t>(node["z"].val());
    int32_t w = string_to_int<int32_t>(node["w"].val());
    return Vector4i(x, y, z, w);
  } else if (node.is_seq() && node.num_children() == 2) {
    int32_t x = string_to_int<int32_t>(node[0].val());
    int32_t y = string_to_int<int32_t>(node[1].val());
    int32_t z = string_to_int<int32_t>(node[2].val());
    int32_t w = string_to_int<int32_t>(node[3].val());
    return Vector4i(x, y, z, w);
  }
  UtilityFunctions::printerr("Invalid Vector4i format: ", String::utf8(node.val().str, node.val().len));
  return Variant();
}

bool Vector4iYAMLEncoder::set_format(const String& format_str)
{
  if (format_str == "flow") {
    format = Format::FLOW_MAP;
  } else if (format_str == "sequence") {
    format = Format::SEQUENCE;
  } else {
    UtilityFunctions::printerr("Invalid format for Vector4i: ", format_str);
    return false;
  }
  return true;
}

void Vector4iYAMLEncoder::emit_as_flow(ryml::NodeRef& node, const Vector4i& vec) const
{
  node |= ryml::MAP;
  node |= ryml::FLOW_SL;
  node["x"] << int_to_string(vec.x);
  node["y"] << int_to_string(vec.y);
  node["z"] << int_to_string(vec.z);
  node["w"] << int_to_string(vec.w);
}

void Vector4iYAMLEncoder::emit_as_sequence(ryml::NodeRef& node, const Vector4i& vec) const
{
  node |= ryml::SEQ;
  node |= ryml::FLOW_SL;
  node.append_child() << int_to_string(vec.x);
  node.append_child() << int_to_string(vec.y);
  node.append_child() << int_to_string(vec.z);
  node.append_child() << int_to_string(vec.w);
}

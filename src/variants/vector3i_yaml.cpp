#include "vector3i_yaml.h"
#include "util_numeric.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include <iomanip>
#include <sstream>

using namespace godot;

const char* Vector3iYAMLEncoder::get_tag() const
{
  return "Vector3i";
}

void Vector3iYAMLEncoder::encode(ryml::NodeRef& node, const Variant& v) const
{
  Vector3i vec = v.operator Vector3i();
  switch (format) {
    case Format::FLOW_MAP:
      emit_as_flow(node, vec);
      break;
    case Format::SEQUENCE:
      emit_as_sequence(node, vec);
      break;
  }
}

Variant Vector3iYAMLEncoder::decode(const ryml::ConstNodeRef& node) const
{
  if (node.is_map()) {
    int32_t x = string_to_int<int32_t>(node["x"].val());
    int32_t y = string_to_int<int32_t>(node["y"].val());
    int32_t z = string_to_int<int32_t>(node["z"].val());
    return Vector3i(x, y, z);
  } else if (node.is_seq() && node.num_children() == 2) {
    int32_t x = string_to_int<int32_t>(node[0].val());
    int32_t y = string_to_int<int32_t>(node[1].val());
    int32_t z = string_to_int<int32_t>(node[2].val());
    return Vector3i(x, y, z);
  }
  throw YAMLException("invalid Vector3i format - " + String::utf8(node.val().str, node.val().len));
}

bool Vector3iYAMLEncoder::set_format(const String& format_str)
{
  if (format_str == "flow") {
    format = Format::FLOW_MAP;
  } else if (format_str == "sequence") {
    format = Format::SEQUENCE;
  } else {
    UtilityFunctions::printerr("YAML error: invalid format for Vector3i - ", format_str);
    return false;
  }
  return true;
}

void Vector3iYAMLEncoder::emit_as_flow(ryml::NodeRef& node, const Vector3i& vec) const
{
  node |= ryml::MAP;
  node |= ryml::FLOW_SL;
  node["x"] << int_to_string(vec.x);
  node["y"] << int_to_string(vec.y);
  node["z"] << int_to_string(vec.z);
}

void Vector3iYAMLEncoder::emit_as_sequence(ryml::NodeRef& node, const Vector3i& vec) const
{
  node |= ryml::SEQ;
  node |= ryml::FLOW_SL;
  node.append_child() << int_to_string(vec.x);
  node.append_child() << int_to_string(vec.y);
  node.append_child() << int_to_string(vec.z);
}

#include "vector3_yaml.h"
#include "util_numeric.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include <iomanip>
#include <sstream>

using namespace godot;

const char* Vector3YAMLEncoder::get_tag() const
{
  return "!!Vector3";
}

void Vector3YAMLEncoder::encode(ryml::NodeRef& node, const Variant& v) const
{
  Vector3 vec = v.operator Vector3();
  switch (format) {
    case Format::FLOW_MAP:
      emit_as_flow(node, vec);
      break;
    case Format::SEQUENCE:
      emit_as_sequence(node, vec);
      break;
  }
}

Variant Vector3YAMLEncoder::decode(const ryml::ConstNodeRef& node) const
{
  if (node.is_map()) {
    real_t x = string_to_float<real_t>(node["x"].val());
    real_t y = string_to_float<real_t>(node["y"].val());
    real_t z = string_to_float<real_t>(node["z"].val());
    return Vector3(x, y, z);
  } else if (node.is_seq() && node.num_children() == 2) {
    real_t x = string_to_float<real_t>(node[0].val());
    real_t y = string_to_float<real_t>(node[1].val());
    real_t z = string_to_float<real_t>(node[2].val());
    return Vector3(x, y, z);
  }
  UtilityFunctions::printerr("Invalid Vector3 format: ", String::utf8(node.val().str, node.val().len));
  return Variant();
}

bool Vector3YAMLEncoder::set_format(const String& format_str)
{
  if (format_str == "flow") {
    format = Format::FLOW_MAP;
  } else if (format_str == "sequence") {
    format = Format::SEQUENCE;
  } else {
    UtilityFunctions::printerr("Invalid format for Vector3: ", format_str);
    return false;
  }
  return true;
}

void Vector3YAMLEncoder::emit_as_flow(ryml::NodeRef& node, const Vector3& vec) const
{
  node |= ryml::MAP;
  node |= ryml::FLOW_SL;
  node["x"] << float_to_string(vec.x);
  node["y"] << float_to_string(vec.y);
  node["z"] << float_to_string(vec.z);
}

void Vector3YAMLEncoder::emit_as_sequence(ryml::NodeRef& node, const Vector3& vec) const
{
  node |= ryml::SEQ;
  node |= ryml::FLOW_SL;
  node.append_child() << float_to_string(vec.x);
  node.append_child() << float_to_string(vec.y);
  node.append_child() << float_to_string(vec.z);
}

#include "vector4_yaml.h"
#include "util_numeric.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

Vector4YAMLEncoder::Vector4YAMLEncoder(YAML* yaml) :
        YAMLEncoder(yaml) { }

void Vector4YAMLEncoder::encode(ryml::NodeRef& node, const Variant& v) const
{
  Vector4 vec = v.operator Vector4();
  switch (format) {
    case Format::FLOW_MAP:
      emit_as_flow(node, vec);
      break;
    case Format::SEQUENCE:
      emit_as_sequence(node, vec);
      break;
  }
}

Variant Vector4YAMLEncoder::decode(const ryml::ConstNodeRef& node) const
{
  if (node.is_map()) {
    real_t x = string_to_float<real_t>(node["x"].val());
    real_t y = string_to_float<real_t>(node["y"].val());
    real_t z = string_to_float<real_t>(node["z"].val());
    real_t w = string_to_float<real_t>(node["w"].val());
    return Vector4(x, y, z, w);
  } else if (node.is_seq() && node.num_children() == 2) {
    real_t x = string_to_float<real_t>(node[0].val());
    real_t y = string_to_float<real_t>(node[1].val());
    real_t z = string_to_float<real_t>(node[2].val());
    real_t w = string_to_float<real_t>(node[3].val());
    return Vector4(x, y, z, w);
  }
  throw YAMLException("invalid Vector4 format - " + String::utf8(node.val().str, node.val().len));
}

bool Vector4YAMLEncoder::set_format(const String& format_str)
{
  if (format_str == "flow") {
    format = Format::FLOW_MAP;
  } else if (format_str == "sequence") {
    format = Format::SEQUENCE;
  } else {
    UtilityFunctions::printerr("YAML error: invalid format for Vector4 - ", format_str);
    return false;
  }
  return true;
}

void Vector4YAMLEncoder::emit_as_flow(ryml::NodeRef& node, const Vector4& vec) const
{
  node |= ryml::MAP;
  node |= ryml::FLOW_SL;
  node["x"] << float_to_string(vec.x);
  node["y"] << float_to_string(vec.y);
  node["z"] << float_to_string(vec.z);
  node["w"] << float_to_string(vec.w);
}

void Vector4YAMLEncoder::emit_as_sequence(ryml::NodeRef& node, const Vector4& vec) const
{
  node |= ryml::SEQ;
  node |= ryml::FLOW_SL;
  node.append_child() << float_to_string(vec.x);
  node.append_child() << float_to_string(vec.y);
  node.append_child() << float_to_string(vec.z);
  node.append_child() << float_to_string(vec.w);
}

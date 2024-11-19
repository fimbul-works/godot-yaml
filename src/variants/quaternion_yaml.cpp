#include "quaternion_yaml.h"
#include "../util_numeric.h"
#include "../yaml_exception.h"

using namespace godot;

QuaternionVariantConverter::QuaternionVariantConverter(YAML* yaml) :
        VariantConverter(yaml) { }

void QuaternionVariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  Quaternion quat = v.operator Quaternion();
  switch (format) {
    case Format::FLOW_MAP:
      emit_as_flow(node, quat);
      break;
    case Format::SEQUENCE:
      emit_as_sequence(node, quat);
      break;
  }
}

void QuaternionVariantConverter::emit_as_flow(ryml::NodeRef& node, const Quaternion& quat) const
{
  node |= ryml::MAP;
  node |= ryml::FLOW_SL;
  node["x"] << float_to_string(quat.x);
  node["y"] << float_to_string(quat.y);
  node["z"] << float_to_string(quat.z);
  node["w"] << float_to_string(quat.w);
}

void QuaternionVariantConverter::emit_as_sequence(ryml::NodeRef& node, const Quaternion& quat) const
{
  node |= ryml::SEQ;
  node |= ryml::FLOW_SL;
  node.append_child() << float_to_string(quat.x);
  node.append_child() << float_to_string(quat.y);
  node.append_child() << float_to_string(quat.z);
  node.append_child() << float_to_string(quat.w);
}

Variant QuaternionVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  try {
    if (node.is_map()) {
      if (!node.has_child("x") || !node.has_child("y") || !node.has_child("z") || !node.has_child("w")) {
        throw YAMLException::create_missing_field("Quaternion", "x, y, z, w");
      }

      real_t x = string_to_float<real_t>(node["x"].val());
      real_t y = string_to_float<real_t>(node["y"].val());
      real_t z = string_to_float<real_t>(node["z"].val());
      real_t w = string_to_float<real_t>(node["w"].val());
      return Quaternion(x, y, z, w);
    } else if (node.is_seq()) {
      if (node.num_children() != 4) {
        throw YAMLException::create_invalid_sequence_length("Quaternion", 4);
      }

      real_t x = string_to_float<real_t>(node[0].val());
      real_t y = string_to_float<real_t>(node[1].val());
      real_t z = string_to_float<real_t>(node[2].val());
      real_t w = string_to_float<real_t>(node[3].val());
      return Quaternion(x, y, z, w);
    }
    throw YAMLException::create_invalid_format("Quaternion");

  } catch (const YAMLException&) {
    throw; // Re-throw YAML exceptions
  } catch (const std::exception& e) {
    throw YAMLException(String("Failed to decode Quaternion: ") + e.what());
  }
}

bool QuaternionVariantConverter::set_format(const String& format_str)
{
  if (format_str == "flow") {
    format = Format::FLOW_MAP;
    return true;
  } else if (format_str == "sequence") {
    format = Format::SEQUENCE;
    return true;
  }
  return false;
}

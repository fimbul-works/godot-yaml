#include "quaternion_yaml.h"
#include "../exception.h"
#include "../util_numeric.h"
#include "../variant_converter_registry.h"

using namespace godot;

void QuaternionVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const
{
  const Quaternion quat = v.operator Quaternion();
  YAMLFormat::Format fmt = format.get_format(Variant::QUATERNION);

  if (fmt == YAMLFormat::SEQUENCE || fmt == YAMLFormat::FLOW_SEQUENCE) {
    emit_as_sequence(node, quat, format);
  } else {
    emit_as_map(node, quat, format);
  }
}

void QuaternionVariantConverter::emit_as_map(ryml::NodeRef& node, const Quaternion& quat, const YAMLFormat::View& format) const
{
  node |= ryml::MAP;

  if (format.get_format(Variant::QUATERNION) == YAMLFormat::FLOW_MAP) {
    node |= ryml::FLOW_SL;
  }

  node["x"] << float_to_string(quat.x);
  node["y"] << float_to_string(quat.y);
  node["z"] << float_to_string(quat.z);
  node["w"] << float_to_string(quat.w);
}

void QuaternionVariantConverter::emit_as_sequence(ryml::NodeRef& node, const Quaternion& quat, const YAMLFormat::View& format) const
{
  node |= ryml::SEQ;

  if (format.get_format(Variant::QUATERNION) == YAMLFormat::FLOW_SEQUENCE) {
    node |= ryml::FLOW_SL;
  }

  node.append_child() << float_to_string(quat.x);
  node.append_child() << float_to_string(quat.y);
  node.append_child() << float_to_string(quat.z);
  node.append_child() << float_to_string(quat.w);
}

Variant QuaternionVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  try {
    if (node.is_map()) {
      return decode_from_map(node);
    } else if (node.is_seq()) {
      return decode_from_sequence(node);
    }
    throw YAMLException::create_invalid_format("Quaternion");
  } catch (const YAMLException&) {
    throw;
  } catch (const std::exception& e) {
    throw YAMLException::create_decode_error("Quaternion", e.what());
  }
}

Variant QuaternionVariantConverter::decode_from_map(const ryml::ConstNodeRef& node) const
{
  if (!node.has_child("x") || !node.has_child("y") || !node.has_child("z") || !node.has_child("w")) {
    throw YAMLException::create_missing_field("Quaternion", "x, y, z, w");
  }

  return Quaternion(
          string_to_float<real_t>(node["x"].val()),
          string_to_float<real_t>(node["y"].val()),
          string_to_float<real_t>(node["z"].val()),
          string_to_float<real_t>(node["w"].val()));
}

Variant QuaternionVariantConverter::decode_from_sequence(const ryml::ConstNodeRef& node) const
{
  if (node.num_children() != 4) {
    throw YAMLException::create_invalid_sequence_length("Quaternion", 4);
  }

  return Quaternion(
          string_to_float<real_t>(node[0].val()),
          string_to_float<real_t>(node[1].val()),
          string_to_float<real_t>(node[2].val()),
          string_to_float<real_t>(node[3].val()));
}

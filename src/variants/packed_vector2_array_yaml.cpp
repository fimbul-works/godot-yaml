#include "packed_vector2_array_yaml.h"
#include "../yaml_exception.h"

using namespace godot;

PackedVector2ArrayVariantConverter::PackedVector2ArrayVariantConverter(YAML* yaml) :
        VariantConverter(yaml)
{
  vec_encoder = new Vector2VariantConverter(yaml);
  vec_encoder->set_format("flow"); // Use flow format for Vector2 components
}

PackedVector2ArrayVariantConverter::~PackedVector2ArrayVariantConverter()
{
  delete vec_encoder;
}

void PackedVector2ArrayVariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  PackedVector2Array array = v.operator PackedVector2Array();

  node |= ryml::SEQ;

  if (array.size() == 0) {
    return; // Empty sequence
  }

  node |= ryml::FLOW_SL;

  for (int i = 0; i < array.size(); ++i) {
    ryml::NodeRef vec_node = node.append_child();
    vec_encoder->encode(vec_node, array[i]);
  }
}

Variant PackedVector2ArrayVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (!node.is_seq()) {
    throw YAMLException::create_invalid_format("PackedVector2Array");
  }

  PackedVector2Array array;
  int size = node.num_children();
  array.resize(size);

  for (int i = 0; i < size; ++i) {
    try {
      Vector2 vec = vec_encoder->decode(node[i]);
      array.set(i, vec);
    } catch (const std::exception& e) {
      throw YAMLException(String("Failed to decode Vector2 at index ") + String::num_int64(i) + ": " + e.what());
    }
  }

  return array;
}

bool PackedVector2ArrayVariantConverter::set_format(const String& format_str)
{
  // Format setting is delegated to the Vector2 encoder
  return vec_encoder->set_format(format_str);
}

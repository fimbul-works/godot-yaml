#include "packed_vector3_array_yaml.h"
#include "../converter_factory.h"
#include "../exception.h"

using namespace godot;

PackedVector3ArrayVariantConverter::PackedVector3ArrayVariantConverter(ConverterFactory* factory) :
        vec3_converter(factory->create_converter_as<Vector3VariantConverter>(Variant::VECTOR3))
{
  ERR_FAIL_NULL(vec3_converter);
}

void PackedVector3ArrayVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const YAMLStyle::View& style) const
{
  const PackedVector3Array array = v.operator PackedVector3Array();
  node |= ryml::SEQ;

  if (array.size() == 0) {
    return; // Empty sequence
  }

  // Flow style
  style.apply_flow_style(node);

  // Get shared item style if it exists (key "_items" is a convention for shared array item styling)
  YAMLStyle::View shared_item_style;
  if (style.is_valid()) {
    shared_item_style = style.get_child("_items");
  }

  for (int i = 0; i < array.size(); ++i) {
    ryml::NodeRef vec_node = node.append_child();

    YAMLStyle::View item_style;
    if (style.is_valid()) {
      // Check for individual item style
      const String idx = String::num_int64(i);
      if (style.has_child(idx)) {
        item_style = style.get_child(idx);
      }

      // Fall back to shared style
      if (!item_style.is_valid()) {
        item_style = shared_item_style;
      }
    }

    vec3_converter->encode(vec_node, array[i], item_style);
  }
}

Variant PackedVector3ArrayVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (!node.is_seq()) {
    throw YAMLException::create_invalid_format("PackedVector3Array");
  }

  PackedVector3Array array;
  const size_t size = node.num_children();
  array.resize(size);

  if (size > 0) {
    for (size_t i = 0; i < size; ++i) {
      try {
        Vector3 vec3 = vec3_converter->decode(node[i]);
        array.set(i, vec3);
      } catch (const std::exception& e) {
        throw YAMLException::create_decode_error(String("Vector3 at index " + String::num_uint64(i)).utf8().get_data(), e.what());
      }
    }
  }

  return array;
}

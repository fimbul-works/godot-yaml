#include "packed_vector3_array_yaml.h"
#include "../variant_converter_registry.h"
#include "../yaml_exception.h"

using namespace godot;

void PackedVector3ArrayVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const
{
  const PackedVector3Array array = v.operator PackedVector3Array();
  node |= ryml::SEQ;

  if (array.size() == 0) {
    return; // Empty sequence
  }

  node |= ryml::FLOW_SL;
  const auto* vec3_converter = get_vec3_converter();

  for (int i = 0; i < array.size(); ++i) {
    ryml::NodeRef vec_node = node.append_child();
    vec3_converter->encode(vec_node, array[i], format);
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
    const auto* vec3_converter = get_vec3_converter();

    for (size_t i = 0; i < size; ++i) {
      try {
        Vector3 vec3 = vec3_converter->decode(node[i]);
        array.set(i, vec3);
      } catch (const std::exception& e) {
        throw YAMLException(String("Failed to decode Vector3 at index ") + String::num_uint64(i) + ": " + e.what());
      }
    }
  }

  return array;
}

const VariantConverter* PackedVector3ArrayVariantConverter::get_vec3_converter() const
{
  const auto* converter = VariantConverterRegistry::get_converter(Variant::VECTOR3);
  if (!converter) {
    throw YAMLException("Vector3 converter not found in registry");
  }
  return converter;
}

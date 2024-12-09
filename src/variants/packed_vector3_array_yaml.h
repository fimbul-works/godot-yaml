#ifndef PACKED_VECTOR3_ARRAY_YAML_H
#define PACKED_VECTOR3_ARRAY_YAML_H

#include "../converter_factory.h"
#include "../variant_converter.h"
#include "vector3_yaml.h"

namespace godot {

class PackedVector3ArrayVariantConverter : public VariantConverter {
  public:
  explicit PackedVector3ArrayVariantConverter(ConverterFactory* factory);

  DEFINE_YAML_TAG("PackedVector3Array", Variant::PACKED_VECTOR3_ARRAY)

  void encode(ryml::NodeRef& node, const Variant& v, const YAMLStyle::View& style) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  private:
  std::unique_ptr<Vector3VariantConverter> vec3_converter;
};

} // namespace godot

#endif // PACKED_VECTOR3_ARRAY_YAML_H

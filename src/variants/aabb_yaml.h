// variants/aabb_yaml.h
#ifndef AABB_YAML_H
#define AABB_YAML_H

#include "../variant_converter.h"
#include "vector3_yaml.h"

namespace godot {

class AABBVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("AABB", Variant::AABB)

  AABBVariantConverter(YAML* yaml);
  ~AABBVariantConverter();

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;
  bool set_format(const String& format) override;

  private:
  void emit_as_map(ryml::NodeRef& node, const AABB& aabb) const;
  Vector3VariantConverter* vec_encoder;
};

} // namespace godot

#endif // AABB_YAML_H

#ifndef AABB_YAML_H
#define AABB_YAML_H

#include "../variant_converter.h"

namespace godot {

class AABBVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("AABB", Variant::AABB)

  void encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  private:
  void emit_as_map(ryml::NodeRef& node, const AABB& aabb, const YAMLFormat::View& format) const;
  void emit_as_sequence(ryml::NodeRef& node, const AABB& aabb, const YAMLFormat::View& format) const;
  Variant decode_from_map(const ryml::ConstNodeRef& node) const;
  Variant decode_from_sequence(const ryml::ConstNodeRef& node) const;
};

} // namespace godot

#endif // AABB_YAML_H

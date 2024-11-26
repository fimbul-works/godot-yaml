#ifndef OBJECT_YAML_H
#define OBJECT_YAML_H

#include "../variant_converter.h"

namespace godot {

/**
 * YAML converter for Object type.
 * Currently only supports Resources with valid paths.
 * Other object types will raise an error.
 * Resources are stored as their resource paths.
 */
class ObjectVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("Object", Variant::OBJECT)

  void encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  private:
  bool validate_path(const String& path) const;
};

} // namespace godot

#endif // OBJECT_YAML_H

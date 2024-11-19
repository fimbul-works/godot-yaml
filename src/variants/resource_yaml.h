#ifndef RESOURCE_YAML_H
#define RESOURCE_YAML_H

#include "../variant_converter.h"

namespace godot {

class ResourceVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("Resource", Variant::OBJECT)

  ResourceVariantConverter(YAML* yaml);

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;
  bool set_format(const String& format) override;

  private:
  bool validate_path(const String& path) const;
};

} // namespace godot

#endif // RESOURCE_YAML_H

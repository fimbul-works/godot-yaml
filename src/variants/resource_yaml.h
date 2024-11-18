#ifndef RESOURCE_YAML_H
#define RESOURCE_YAML_H

#include "yaml.h"
#include "yaml_encoder.h"
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/resource.hpp>

namespace godot {

class ResourceVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("Resource", Variant::OBJECT)

  ResourceVariantConverter(YAML* yaml);

  void encode(ryml::NodeRef& node, const Variant& v) const;
  Variant decode(const ryml::ConstNodeRef& node) const;

  bool set_format(const String& format_str);

  YAML* m_yaml;
};

} // namespace godot

#endif // RESOURCE_YAML_H

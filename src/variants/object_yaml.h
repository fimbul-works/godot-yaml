#ifndef OBJECT_YAML_H
#define OBJECT_YAML_H

#include "../style_view.h"
#include "../variant_converter.h"

#include <godot_cpp/classes/resource.hpp>

namespace godot {

class ObjectVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("Object", Variant::OBJECT)

  void encode(ryml::NodeRef& node, const Variant& v, const YAMLStyle::View& style) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  private:
  void emit_resource(ryml::NodeRef& node, const Resource* res, const YAMLStyle::View& style) const;
  void emit_object_properties(ryml::NodeRef& node, const Object* obj, const YAMLStyle::View& style) const;
  void emit_property_value(ryml::NodeRef& node, const String& prop_name, const Variant& value, const YAMLStyle::View& style) const;
  bool should_serialize_property(const Dictionary& prop_info) const;
};

} // namespace godot

#endif // OBJECT_YAML_H

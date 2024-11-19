#ifndef STRING_NAME_YAML_H
#define STRING_NAME_YAML_H

#include "../variant_converter.h"

namespace godot {

class StringNameVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("StringName", Variant::STRING_NAME)

  StringNameVariantConverter(YAML* yaml);

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;
  bool set_format(const String& format) override;
};

} // namespace godot

#endif // STRING_NAME_YAML_H

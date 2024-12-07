#ifndef STRING_NAME_YAML_H
#define STRING_NAME_YAML_H

#include "../variant_converter.h"

namespace godot {

class StringNameVariantConverter : public VariantConverter {
  public:
  DEFINE_YAML_TAG("StringName", Variant::STRING_NAME)

  void encode(ryml::NodeRef& node, const Variant& v, const Ref<YAMLStyle>& style) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  private:
  void emit_as_string(ryml::NodeRef& node, const StringName& str) const;
  Variant decode_from_string(const ryml::csubstr& val) const;
};

} // namespace godot

#endif // STRING_NAME_YAML_H

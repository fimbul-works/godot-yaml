#include "variants.h"
#include "yaml.h"

#include "variants/color_yaml.h"

using namespace godot;

void YAML::register_handlers()
{
  add_variant_handler<Color>("!!Color");
}

template <typename T>
void YAML::add_variant_handler(const std::string& tag)
{
  UtilityFunctions::print("Adding variant handler: ", String(tag.c_str()));

  Variant::Type type = Variant(T()).get_type();
  type_to_tag[type] = tag;

  parse_handlers[tag] = [](const ryml::ConstNodeRef& node) -> Variant {
    return variants::parse<T>(node);
  };

  emit_handlers[tag] = [](ryml::NodeRef& node, const Variant& v) {
    variants::emit<T>(node, v);
  };
}

template <typename T>
Variant YAML::parse_variant(const ryml::ConstNodeRef& node)
{
  return variants::parse<T>(node);
}

template <typename T>
void YAML::emit_variant(ryml::NodeRef& node, const Variant& v)
{
  variants::emit<T>(node, v);
}

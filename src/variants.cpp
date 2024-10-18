#include "yaml.h"

#include "variants/color_yaml.h"

using namespace godot;

void YAML::register_types()
{
  register_type<Color>();
}

template <typename T>
void YAML::register_type()
{
  Variant::Type type = Variant(T()).get_type();
  const char* tag = yaml::YAMLEncoder<T>::get_tag();
#ifdef GODOT_YAML_DEBUG
  UtilityFunctions::print("Registered type: ", type_string(typeof(type)));
#endif

  type_to_tag[type] = tag;

  decoders[tag] = [](const ryml::ConstNodeRef& node) -> Variant {
    T value;
    if (yaml::YAMLEncoder<T>::decode(node, value)) {
      return Variant(value);
    }
    return Variant();
  };

  encoders[type] = [](ryml::NodeRef& node, const Variant& v) {
    yaml::YAMLEncoder<T>::encode(node, v.operator T());
  };
}

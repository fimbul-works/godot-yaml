#ifndef YAML_ENCODER_H
#define YAML_ENCODER_H

#include <godot_cpp/variant/variant.hpp>
#include <ryml.hpp>

namespace godot {
namespace yaml {

  template <typename T>
  struct YAMLEncoder {
    static void encode(ryml::NodeRef node, const T& value);
    static bool decode(const ryml::ConstNodeRef& node, T& out_value);
    static const char* get_tag();
  };

} // namespace yaml
} // namespace godot

#endif // YAML_ENCODER_H

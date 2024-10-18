#ifndef COLOR_YAML_H
#define COLOR_YAML_H

#include "yaml_encoder.h"
#include <godot_cpp/variant/color.hpp>

namespace godot {
namespace yaml {

  template <>
  struct YAMLEncoder<Color> {
    static void encode(ryml::NodeRef node, const Color& color);
    static bool decode(const ryml::ConstNodeRef& node, Color& out_color);
    static const char* get_tag() { return "!!Color"; }
  };

} // namespace yaml
} // namespace godot

#endif // COLOR_YAML_H

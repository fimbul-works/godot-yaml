#ifndef COLOR_YAML_H
#define COLOR_YAML_H

#include "variants.h"
#include <godot_cpp/variant/color.hpp>

namespace godot {
namespace variants {

  template <>
  Color parse<Color>(const ryml::ConstNodeRef& node);

  template <>
  void emit<Color>(ryml::NodeRef& node, const Color& color);

} // namespace variants
} // namespace godot

#endif // COLOR_YAML_H

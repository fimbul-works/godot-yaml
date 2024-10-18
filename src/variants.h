#ifndef VARIANTS_H
#define VARIANTS_H

#include <godot_cpp/variant/variant.hpp>
#include <ryml.hpp>

namespace godot {
namespace variants {

  inline std::string node_value_to_string(const c4::yml::ConstNodeRef& node)
  {
    return std::string(node.val().unquoted().str, node.val().unquoted().len);
  }

  template <typename T>
  T parse(const ryml::ConstNodeRef& node);

  template <typename T>
  void emit(ryml::NodeRef& node, const T& value);

  template <typename T, typename = void>
  struct has_yaml_specialization : std::false_type { };

  template <typename T>
  struct has_yaml_specialization<T, std::void_t<decltype(parse<T>(std::declval<const ryml::ConstNodeRef&>())), decltype(emit<T>(std::declval<ryml::NodeRef&>(), std::declval<const T&>()))>> : std::true_type { };

} // namespace variants
} // namespace godot

#endif // VARIANTS_H

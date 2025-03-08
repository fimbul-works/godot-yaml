#ifndef YAML_H
#define YAML_H

#include "result.h"
#include "style.h"

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/script.hpp>
#include <godot_cpp/variant/variant.hpp>

namespace godot {

class YAML : public RefCounted {
  GDCLASS(YAML, RefCounted);

  protected:
  static void _bind_methods();

  public:
  // Forward-declare inner classes, defined in validator.h, parser.h, and emitter.h
  class Validator;
  class Parser;
  class Emitter;

  static String version();
  static Ref<YAMLResult> validate(const String& input);
  static Ref<YAMLResult> parse(const String& input, const bool detect_style = false);
  static Ref<YAMLResult> stringify(const Variant& input, const Ref<YAMLStyle>& format);
  static Ref<YAMLStyle> create_style();

  static void register_class(Ref<Script> p_class, const Variant& p_to_dict, const Variant& p_from_dict);
  static bool has_registered_class(const String& class_name);
};

} // namespace godot

#endif // YAML_H

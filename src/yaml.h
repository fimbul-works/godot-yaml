#ifndef YAML_H
#define YAML_H

#include "emitter.h"
#include "parser.h"
#include "result.h"
#include "style.h"

#include <godot_cpp/core/class_db.hpp>

namespace godot {

class YAML : public RefCounted {
  GDCLASS(YAML, RefCounted);

  protected:
  static void _bind_methods();

  YAML() = default;
  ~YAML() = default;

  public:
  static String version();
  static Ref<YAMLResult> parse(const String& input, const bool detect_style = false);
  static Ref<YAMLResult> emit(const Variant& input, const Ref<YAMLStyle>& format);
  static Ref<YAMLStyle> create_style();
};

} // namespace godot

#endif // YAML_H

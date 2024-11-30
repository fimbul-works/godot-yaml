#ifndef YAML_H
#define YAML_H

#include "emitter.h"
#include "format.h"
#include "parser.h"
#include "result.h"

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
  static Ref<YAMLResult> parse(const String& input);
  static Ref<YAMLResult> emit(const Variant& input, const Ref<YAMLFormat>& format);
  static Ref<YAMLFormat> format();
};

} // namespace godot

#endif // YAML_H

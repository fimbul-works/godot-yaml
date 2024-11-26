#ifndef YAML_H
#define YAML_H

#include "yaml_emitter.h"
#include "yaml_format.h"
#include "yaml_parser.h"
#include "yaml_result.h"

#include <godot_cpp/core/class_db.hpp>

namespace godot {

class YAML : public Object {
  GDCLASS(YAML, Object)

  protected:
  static void _bind_methods();
  // Keep protected so class cannot be initialized
  YAML();
  ~YAML();

  public:
  static String version();

  // For custom parser/emitter instances
  static Ref<YAMLParser> create_parser();
  static Ref<YAMLEmitter> create_emitter();

  // Shorthand methods
  static Ref<YAMLResult> parse(const String& input);
  static Ref<YAMLResult> emit(const Variant& input, const Ref<YAMLFormat>& format);
};

} // namespace godot

#endif // YAML_H

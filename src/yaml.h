#ifndef YAML_H
#define YAML_H

// #include "variant_converter.h"
#include "yaml_emitter.h"
#include "yaml_parser.h"
#include "yaml_result.h"

#include <godot_cpp/core/class_db.hpp>

namespace godot {

class YAML : public Object {
  GDCLASS(YAML, Object)

  protected:
  static void _bind_methods();

  private:
  // Default instances for simple parse/emit operations
  Ref<YAMLParser> default_parser;
  Ref<YAMLEmitter> default_emitter;

  public:
  YAML();
  ~YAML();

  String version();

  // Core API methods using default instances
  Ref<YAMLResult> parse(const String& input);
  Ref<YAMLResult> emit(const Variant& input);

  // Create new parser/emitter instances
  Ref<YAMLParser> create_parser() const;
  Ref<YAMLEmitter> create_emitter() const;
};

} // namespace godot

#endif // YAML_H

#ifndef YAML_H
#define YAML_H

#include "variant_converter.h"
#include "yaml_emitter.h"
#include "yaml_parser.h"
#include "yaml_result.h"

#include <godot_cpp/core/class_db.hpp>
#include <memory>
#include <unordered_map>

namespace godot {

class YAML : public Object {
  GDCLASS(YAML, Object)

  protected:
  static void _bind_methods();

  private:
  // Default instances for simple parse/emit operations
  Ref<YAMLParser> default_parser;
  Ref<YAMLEmitter> default_emitter;

  // Type conversion management
  std::unordered_map<String, VariantConverter*> tag_to_converter;
  std::unordered_map<Variant::Type, std::unique_ptr<VariantConverter>> type_to_converter;
  std::unique_ptr<VariantConverter> resource_converter;

  // Friends to allow access to converters
  friend class YAMLParser;
  friend class YAMLEmitter;

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

  // Format configuration (delegates to emitter)
  bool set_format(Variant::Type type, const String& format);
  String get_format(Variant::Type type) const;

  // Internal converter access (for parser/emitter)
  const VariantConverter* get_converter_by_tag(const String& tag) const;
  const VariantConverter* get_converter_by_type(Variant::Type type) const;

  private:
  void register_converter(std::unique_ptr<VariantConverter> converter);
  void register_type_converters();
};

} // namespace godot

#endif // YAML_H

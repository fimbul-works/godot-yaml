#ifndef YAML_EMITTER_H
#define YAML_EMITTER_H

#include "variant_converter_registry.h"
#include "yaml_format.h"
#include "yaml_result.h"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <ryml.hpp>

#include <memory>
#include <mutex>

namespace godot {

class YAMLEmitter : public RefCounted {
  GDCLASS(YAMLEmitter, RefCounted)

  protected:
  static void _bind_methods();

  public:
  YAMLEmitter();
  explicit YAMLEmitter(const Ref<YAMLFormat>& default_format);
  virtual ~YAMLEmitter();

  Ref<YAMLResult> emit(const Variant& input, const Ref<YAMLFormat>& format = YAMLFormat::create_default());

  // Format management
  void set_default_format(const Ref<YAMLFormat>& format);
  Ref<YAMLFormat> get_default_format() const;

  private:
  // Thread safety
  std::mutex emit_mutex;

  // State management
  Ref<YAMLResult> current_result;
  Ref<YAMLFormat> default_format;

  // String handling utilities
  bool needs_block_style(const String& str) const
  {
    return str.contains("\n") || str.contains("\"") || str.begins_with(" ") || str.ends_with(" ") || str.begins_with("#");
  }

  bool is_multiline(const String& str) const
  {
    return str.contains("\n");
  }

  // Core emission methods using immutable View - declarations only
  void emit_value(ryml::NodeRef& node, const Variant& value, const YAMLFormat::View& format);
  void emit_nil(ryml::NodeRef& node);
  void emit_bool(ryml::NodeRef& node, bool value);
  void emit_number(ryml::NodeRef& node, const Variant& value);
  void emit_string(ryml::NodeRef& node, const String& value);
  void emit_array(ryml::NodeRef& node, const Array& array, const YAMLFormat::View& format);
  void emit_dictionary(ryml::NodeRef& node, const Dictionary& dict, const YAMLFormat::View& format);
  void emit_tagged_value(ryml::NodeRef& node, const Variant& value, const YAMLFormat::View& format);

  const VariantConverter* get_converter_for_type(Variant::Type type) const;
};

} // namespace godot

#endif // YAML_EMITTER_H

#ifndef YAML_EMITTER_H
#define YAML_EMITTER_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <mutex>
#include <ryml.hpp>

namespace godot {

class YAMLResult;

class YAMLEmitter : public RefCounted {
  GDCLASS(YAMLEmitter, RefCounted)

  private:
  // Static default formats with thread safety
  static std::unordered_map<Variant::Type, String> default_formats;
  static std::mutex default_formats_mutex;

  // Instance-specific formats
  std::unordered_map<Variant::Type, String> instance_formats;

  protected:
  static void _bind_methods();

  public:
  YAMLEmitter();
  ~YAMLEmitter();

  // Main emitting function
  Ref<YAMLResult> emit(const Variant& input);

  // Instance format configuration
  bool set_format(Variant::Type type, const String& format);
  String get_format(Variant::Type type) const;
  void reset_formats(); // Reset to default formats

  // Static format configuration
  static bool set_default_format(Variant::Type type, const String& format);
  static String get_default_format(Variant::Type type);
  static void reset_default_formats();

  private:
  // Internal emission helpers
  void emit_recursively(ryml::NodeRef& node, const Variant& v, Ref<YAMLResult>& result);
  void emit_map(ryml::NodeRef& node, const Dictionary& dict, Ref<YAMLResult>& result);
  void emit_array(ryml::NodeRef& node, const Array& arr, Ref<YAMLResult>& result);
  void emit_string(ryml::NodeRef& node, const String& str);
  void emit_number(ryml::NodeRef& node, const Variant& v);
  void set_error(Ref<YAMLResult>& result, const String& error, int line = -1, int column = -1);
};

} // namespace godot

#endif // YAML_EMITTER_H

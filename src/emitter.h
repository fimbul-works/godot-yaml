#ifndef YAML_EMITTER_H
#define YAML_EMITTER_H

#include "result.h"
#include "style.h"
#include "variant_converter_registry.h"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <ryml.hpp>

#include <memory>
#include <mutex>

namespace godot {

class YAMLEmitter {
  public:
  YAMLEmitter() = default;
  ~YAMLEmitter() = default;

  Ref<YAMLResult> emit(const Variant& input, const Ref<YAMLStyle>& style = nullptr);

  private:
  std::mutex emit_mutex;
  Ref<YAMLResult> current_result;

  void emit_value(ryml::NodeRef& node, const Variant& value, const Ref<YAMLStyle>& style);
  void emit_nil(ryml::NodeRef& node);
  void emit_bool(ryml::NodeRef& node, bool value);
  void emit_number(ryml::NodeRef& node, const Variant& value, const Ref<YAMLStyle>& style);
  void emit_string(ryml::NodeRef& node, const String& value, const Ref<YAMLStyle>& style);
  void emit_array(ryml::NodeRef& node, const Array& array, const Ref<YAMLStyle>& style);
  void emit_dictionary(ryml::NodeRef& node, const Dictionary& dict, const Ref<YAMLStyle>& style);
  void emit_tagged_value(ryml::NodeRef& node, const Variant& value, const Ref<YAMLStyle>& style);
};

} // namespace godot

#endif // YAML_EMITTER_H

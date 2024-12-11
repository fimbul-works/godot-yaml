#ifndef YAML_EMITTER_H
#define YAML_EMITTER_H

#include "converter_factory.h"
#include "result.h"
#include "string_hash.h"
#include "style_view.h"
#include "yaml.h"

#include <godot_cpp/classes/resource.hpp>
#include <ryml.hpp>

#include <memory>
#include <mutex>

namespace godot {

class YAML::Emitter {
  public:
  Emitter();
  ~Emitter() = default;

  // Non-copyable
  Emitter(const Emitter&) = delete;
  Emitter& operator=(const Emitter&) = delete;

  Ref<YAMLResult> emit(const Variant& input, const YAMLStyle::View& style);

  private:
  // Ryml setup
  ryml::Callbacks callbacks;
  std::unique_ptr<ryml::EventHandlerTree> evt_handler;
  ryml::Tree tree;

  // Owned converters
  ConverterFactory factory;
  std::unordered_map<Variant::Type, std::unique_ptr<VariantConverter>> type_converters;
  std::unordered_map<String, VariantConverter*, StringHasher, StringEqual> tag_converters;

  // Current state
  Ref<YAMLResult> current_result;
  YAMLStyle::View current_style;

  // Error handling
  static void error_callback(const char* msg, size_t len, ryml::Location loc, void* user_data);

  // Initialize converters
  void init_converters();

  // Converter access helpers
  VariantConverter* get_converter_for_type(Variant::Type type) const;
  VariantConverter* get_converter_for_tag(const String& tag) const;

  // Keep string data alive for the duration of emission
  std::vector<std::string> string_storage;

  // Helper to store a string and get a safe view
  ryml::csubstr store_string(const String& str);

  void reset();

  // Core emission methods
  void emit_value(ryml::NodeRef& node, const Variant& value, const YAMLStyle::View& style);
  void emit_nil(ryml::NodeRef& node);
  void emit_bool(ryml::NodeRef& node, bool value);
  void emit_number(ryml::NodeRef& node, const Variant& value, const YAMLStyle::View& style);
  void emit_string(ryml::NodeRef& node, const String& value, const YAMLStyle::View& style);
  void emit_array(ryml::NodeRef& node, const Array& array, const YAMLStyle::View& style);
  void emit_dictionary(ryml::NodeRef& node, const Dictionary& dict, const YAMLStyle::View& style);

  void emit_object(ryml::NodeRef& node, const Variant& v, const YAMLStyle::View& style);
  void emit_resource(ryml::NodeRef& node, const Resource* res, const YAMLStyle::View& style);
  void emit_object_properties(ryml::NodeRef& node, const Object* obj, const YAMLStyle::View& style);
  void emit_property_value(ryml::NodeRef& node, const String& prop_name, const Variant& value, const YAMLStyle::View& style);
  bool should_serialize_property(const Dictionary& prop_info);
};

} // namespace godot

#endif // YAML_EMITTER_H

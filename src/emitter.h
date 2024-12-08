#ifndef YAML_EMITTER_H
#define YAML_EMITTER_H

#include "reflection.h"
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
  // Thread-local instance containing all state
  struct EmitterInstance {
    EmitterInstance();
    ~EmitterInstance() = default;

    // Ryml setup
    ryml::Callbacks m_callbacks;
    std::unique_ptr<ryml::EventHandlerTree> m_evt_handler;
    ryml::Tree m_tree;

    // Current state
    Ref<YAMLResult> current_result;
    Ref<YAMLStyle> current_style;

    // Error handling
    static void error_callback(const char* msg, size_t len, ryml::Location loc, void* user_data);

    // Core emission methods
    void emit_value(ryml::NodeRef& node, const Variant& value, const Ref<YAMLStyle>& style);
    void emit_nil(ryml::NodeRef& node);
    void emit_bool(ryml::NodeRef& node, bool value);
    void emit_number(ryml::NodeRef& node, const Variant& value, const Ref<YAMLStyle>& style);
    void emit_string(ryml::NodeRef& node, const String& value, const Ref<YAMLStyle>& style);
    void emit_array(ryml::NodeRef& node, const Array& array, const Ref<YAMLStyle>& style);
    void emit_dictionary(ryml::NodeRef& node, const Dictionary& dict, const Ref<YAMLStyle>& style);

    // Object handling methods
    void emit_object(ryml::NodeRef& node, const Object* obj, const Ref<YAMLStyle>& style);
    void emit_resource(ryml::NodeRef& node, const Resource* res, const Ref<YAMLStyle>& style);
    void emit_object_properties(ryml::NodeRef& node, const Object* obj, const Ref<YAMLStyle>& style);

    // Helper methods
    bool should_serialize_property(const Dictionary& prop_info) const;
    void emit_property_value(ryml::NodeRef& node, const String& prop_name,
            const Variant& value, const Ref<YAMLStyle>& style);
  };

  static thread_local EmitterInstance t_emitter_instance;
};

} // namespace godot

#endif // YAML_EMITTER_H

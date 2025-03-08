#include "class_registry.h"
#include <godot_cpp/classes/script.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

// Initialize static members
std::mutex YAMLClassRegistry::registry_mutex;
std::unordered_map<String, YAMLClassRegistry::ClassInfo, StringHasher, StringEqual> YAMLClassRegistry::class_registry;

// Register a class with the registry
void YAMLClassRegistry::register_class(Ref<Script> p_class, const Variant& p_to_dict, const Variant& p_from_dict)
{
  if (!p_class.is_valid()) {
    ERR_PRINT("Cannot register null class");
    return;
  }

  // Find out the real class name
  const StringName class_name = p_class->get_global_name();
  if (class_name.is_empty()) {
    ERR_PRINT(vformat("Cannot register class %s - could not determine global class name", p_class->get_class()));
    return;
  }

  // Prevent duplicats
  if (has_class(class_name)) {
    ERR_PRINT(vformat("Class %s is already registered", class_name));
    return;
  }

  // Check static from_dict method
  const StringName from_dict = p_from_dict.get_type() == Variant::STRING || p_from_dict.get_type() == Variant::STRING_NAME ? p_from_dict : "from_dict";
  if (!p_class->has_method(from_dict)) {
    ERR_PRINT(vformat("Static method '%s' not found in class %s", from_dict, class_name));
    return;
  }

  if (!p_class->can_instantiate()) {
    ERR_PRINT(vformat("Cannot instantiate class %s", class_name));
    return;
  }

  // Create an instance
  Object* instance = ClassDB::instantiate(p_class->get_instance_base_type());
  if (!instance) {
    ERR_PRINT(vformat("Failed to instantiate class %s", class_name));
    return;
  }
  // Attach the script
  instance->set_script(p_class);

  // Check instance method
  const StringName to_dict = p_to_dict.get_type() == Variant::STRING || p_to_dict.get_type() == Variant::STRING_NAME ? p_to_dict : "to_dict";
  if (!instance->has_method(to_dict)) {
    ERR_PRINT(vformat("Method '%s' not found in class %s", to_dict, class_name));
    memdelete(instance);
    return;
  }

  // Clean up memory
  memdelete(instance);

  // Create the class info
  ClassInfo info;
  info.script_class = p_class;
  info.to_dict_method = to_dict;
  info.from_dict_method = from_dict;

  // Add to registry with thread safety
  {
    std::lock_guard<std::mutex> lock(registry_mutex);
    class_registry[class_name] = info;
  }

  UtilityFunctions::print(vformat("Registered YAML class: %s", class_name));
}

// Check if a class is registered
bool YAMLClassRegistry::has_class(const String& tag_name)
{
  std::lock_guard<std::mutex> lock(registry_mutex);
  return class_registry.find(tag_name) != class_registry.end();
}

// Get class info by tag name
YAMLClassRegistry::ClassInfo YAMLClassRegistry::get_class_info(const String& tag_name)
{
  std::lock_guard<std::mutex> lock(registry_mutex);
  auto it = class_registry.find(tag_name);
  if (it != class_registry.end()) {
    return it->second;
  }
  return ClassInfo(); // Return empty info if not found
}

std::unordered_map<String, YAMLClassRegistry::ClassInfo, StringHasher, StringEqual> YAMLClassRegistry::get_all_classes()
{
  std::lock_guard<std::mutex> lock(registry_mutex);
  return class_registry; // Return a copy
}

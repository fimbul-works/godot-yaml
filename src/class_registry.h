#ifndef YAML_CLASS_REGISTRY_H
#define YAML_CLASS_REGISTRY_H

#include "string_hash.h"
#include <godot_cpp/classes/script.hpp>
#include <godot_cpp/variant/string.hpp>
#include <mutex>
#include <unordered_map>

namespace godot {

class YAMLClassRegistry {
  public:
  struct ClassInfo {
    Ref<Script> script_class;
    StringName to_dict_method;
    StringName from_dict_method;
  };

  static void register_class(Ref<Script> p_class, const Variant& p_to_dict, const Variant& p_from_dict);

  static bool has_class(const String& class_name);

  static ClassInfo get_class_info(const String& class_name);

  static std::unordered_map<String, ClassInfo, StringHasher, StringEqual> get_all_classes();

  private:
  static std::mutex registry_mutex;
  static std::unordered_map<String, ClassInfo, StringHasher, StringEqual> class_registry;
};

} // namespace godot

#endif // YAML_CLASS_REGISTRY_H

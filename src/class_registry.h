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
		StringName serialize_method;
		StringName deserialize_method;
	};

	static void register_class(Ref<Script> p_class, const Variant &p_serialize, const Variant &p_deserialize);
	static void unregister_class(Ref<Script> p_class);

	static bool has_class(const String &class_name);

	static ClassInfo get_class_info(const String &class_name);

private:
	static String get_script_class(Ref<Script> p_class);
	static std::mutex registry_mutex;
	static std::unordered_map<String, ClassInfo, StringHasher, StringEqual> class_registry;
};

} // namespace godot

#endif // YAML_CLASS_REGISTRY_H

#include "class_registry.hpp"

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/script.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

// Initialize static members
std::mutex YAMLClassRegistry::registry_mutex;
std::unordered_map<String, YAMLClassRegistry::ClassInfo, StringHasher, StringEqual> YAMLClassRegistry::class_registry;

void YAMLClassRegistry::register_class(Ref<Script> p_class, const Variant &p_serialize, const Variant &p_deserialize, const Variant &p_tag) {
	const StringName class_name = get_script_class(p_class);
	if (class_name.is_empty()) {
		return;
	}

	// Prevent duplicates
	if (has_class(class_name)) {
		ERR_PRINT(vformat("Class %s is already registered", class_name));
		return;
	}

	// Check static from_dict method
	const StringName deserialize =
			p_deserialize.get_type() == Variant::STRING || p_deserialize.get_type() == Variant::STRING_NAME
			? p_deserialize
			: "deserialize";
	if (!p_class->has_method(deserialize)) {
		ERR_PRINT(vformat("Static method '%s' not found in class %s", deserialize, class_name));
		return;
	}

	if (!p_class->can_instantiate()) {
		ERR_PRINT(vformat("Cannot instantiate class %s", class_name));
		return;
	}

	// Check instance method
	const StringName serialize = p_serialize.get_type() == Variant::STRING || p_serialize.get_type() == Variant::STRING_NAME ? p_serialize : "serialize";

	// Create an instance
	StringName base_type = p_class->get_instance_base_type();

	if (base_type == StringName("Resource")) {
		// For Resource types
		Ref<Resource> res;
		res.instantiate();
		res->set_script(p_class);

		if (!res->has_method(serialize)) {
			ERR_PRINT(vformat("Method '%s' not found in resource class %s", serialize, class_name));
			return;
		}
		// Ref will clean up automatically when it goes out of scope
	} else {
		// For non-Resource types
		Object *instance = ClassDB::instantiate(base_type);
		if (!instance) {
			ERR_PRINT(vformat("Failed to instantiate class %s with base type %s", class_name, base_type));
			return;
		}

		// Attach the script
		instance->set_script(p_class);

		if (!instance->has_method(serialize)) {
			ERR_PRINT(vformat("Method '%s' not found in class %s", serialize, class_name));
			memdelete(instance);
			return;
		}

		// Clean up memory
		memdelete(instance);
	}

	// Create the class info
	const StringName tag = p_tag.get_type() == Variant::STRING || p_tag.get_type() == Variant::STRING_NAME ? StringName(p_tag) : class_name;
	ClassInfo info;
	info.script_class = p_class;
	info.tag = tag.length() ? tag : class_name;
	info.serialize_method = serialize;
	info.deserialize_method = deserialize;

	// Add to registry with thread safety
	{
		std::lock_guard<std::mutex> lock(registry_mutex);
		class_registry[class_name] = info;
		// Add the tag also
		if (class_name != tag && tag.length()) {
			class_registry[tag] = info;
		}
	}

#ifdef GODOT_YAML_DEBUG
	UtilityFunctions::print(vformat("Registered class %s with YAML", class_name));
#endif
}

void YAMLClassRegistry::unregister_class(Ref<Script> p_class) {
	const StringName class_name = get_script_class(p_class);
	if (class_name.is_empty()) {
		return;
	}

	// Prevent duplicates
	if (!has_class(class_name)) {
#ifdef GODOT_YAML_DEBUG
		ERR_PRINT(vformat("Class %s is not registered with YAML", class_name));
#endif
		return;
	}

	// Remove from registry with thread safety
	{
		std::lock_guard<std::mutex> lock(registry_mutex);
		auto class_info = get_class_info(class_name);
		class_registry.erase(class_name);
		// Erase tag also
		if (class_info.tag != class_name && class_info.tag.length()) {
			class_registry.erase(class_info.tag);
		}
	}

#ifdef GODOT_YAML_DEBUG
	UtilityFunctions::print(vformat("Unregistered class %s from YAML", class_name));
#endif
}

bool YAMLClassRegistry::has_class(const String &tag_name) {
	std::lock_guard<std::mutex> lock(registry_mutex);
	return class_registry.find(tag_name) != class_registry.end();
}

YAMLClassRegistry::ClassInfo YAMLClassRegistry::get_class_info(const String &tag_name) {
	std::lock_guard<std::mutex> lock(registry_mutex);
	auto it = class_registry.find(tag_name);
	if (it != class_registry.end()) {
		return it->second;
	}
	return ClassInfo(); // Return empty info if not found
}

String YAMLClassRegistry::get_script_class(Ref<Script> p_class) {
	if (!p_class.is_valid()) {
		ERR_PRINT("Invalid class");
		return "";
	}

	// Find out the real class name
	const StringName class_name = p_class->get_global_name();
	if (class_name.is_empty()) {
		ERR_PRINT(vformat("Cannot register class %s - could not determine global class name", p_class->get_class()));
		return "";
	}

	return class_name;
}

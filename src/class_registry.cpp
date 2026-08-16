#include "class_registry.hpp"
#include "util/timestamp_str.hpp"
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/script.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

YAMLClassRegistry *YAMLClassRegistry::singleton = nullptr;

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

		// Attach script
		res->set_script(p_class);
		bool has_method = res->has_method(serialize);
		res->set_script(Variant());

		if (!has_method) {
			ERR_PRINT(vformat("Method '%s' not found in resource class %s", serialize, class_name));
			return;
		}
	} else {
		// For non-Resource types
		Object *instance = ClassDB::instantiate(base_type);
		if (!instance) {
			ERR_PRINT(vformat("Failed to instantiate class %s with base type %s", class_name, base_type));
			return;
		}

		// Attach script
		instance->set_script(p_class);
		bool has_method = instance->has_method(serialize);
		instance->set_script(Variant());

		memdelete(instance);

		if (!has_method) {
			ERR_PRINT(vformat("Method '%s' not found in class %s", serialize, class_name));
			return;
		}
	}

	// Create the class info
	const String tag = p_tag.get_type() == Variant::STRING || p_tag.get_type() == Variant::STRING_NAME ? String(p_tag) : String(class_name);
	ClassInfo info;
	info.script_class = p_class;
	info.tag = tag.length() ? tag : String(class_name);
	info.serialize_method = serialize;
	info.deserialize_method = deserialize;

	// Add to registry with thread safety
	registry_mutex->lock();
	class_registry[class_name] = info;

	// Add the tag also
	if (class_name != tag && tag.length()) {
		class_registry[tag] = info;
	}
	registry_mutex->unlock();

#ifdef GODOT_YAML_DEBUG
	String timestamp = timestamp_str();
	UtilityFunctions::print(vformat("%s YAML.register_class(%s)", timestamp, class_name));
#endif
}

void YAMLClassRegistry::unregister_class(Ref<Script> p_class) {
	const StringName class_name = get_script_class(p_class);
	if (class_name.is_empty()) {
		return;
	}

	// Remove from registry with thread safety
	registry_mutex->lock();
	auto it = class_registry.find(class_name);
	if (it == class_registry.end()) {
		registry_mutex->unlock();
#ifdef GODOT_YAML_DEBUG
		ERR_PRINT(vformat("Class %s is not registered with YAML", class_name));
#endif
		return;
	}

	String tag = it->second.tag;
	class_registry.erase(it);
	// Erase tag also
	if (tag != class_name && !tag.is_empty()) {
		class_registry.erase(tag);
	}
	registry_mutex->unlock();

#ifdef GODOT_YAML_DEBUG
	UtilityFunctions::print(vformat("Unregistered class %s from YAML", class_name));
#endif
}

bool YAMLClassRegistry::has_class(const String &tag_name) {
	registry_mutex->lock();
	bool result = class_registry.find(tag_name) != class_registry.end();
	registry_mutex->unlock();
	return result;
}

YAMLClassRegistry::ClassInfo YAMLClassRegistry::get_class_info(const String &tag_name) {
	registry_mutex->lock();
	auto it = class_registry.find(tag_name);
	if (it != class_registry.end()) {
		auto info = it->second;
		registry_mutex->unlock();
		return info;
	}
	registry_mutex->unlock();
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

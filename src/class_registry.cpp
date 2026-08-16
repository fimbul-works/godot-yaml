#include "class_registry.hpp"

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/script.hpp>
#include <godot_cpp/classes/time.hpp>
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

//#ifdef GODOT_YAML_DEBUG
#define TICKS_MS 1000
#define TICKS_SECOND (TICKS_MS * 1000)
#define TICKS_MINUTE (TICKS_SECOND * 60)
#define TICKS_HOUR (TICKS_MINUTE * 60)
	uint64_t ticks_usec = Time::get_singleton()->get_ticks_usec();

	uint64_t ticks_hour = UtilityFunctions::floori(ticks_usec / TICKS_HOUR);
	ticks_usec -= ticks_hour * TICKS_HOUR;

	uint64_t ticks_minute = UtilityFunctions::floori(ticks_usec / TICKS_MINUTE);
	ticks_usec -= ticks_minute * TICKS_MINUTE;

	uint64_t ticks_second = UtilityFunctions::floori(ticks_usec / TICKS_SECOND);
	ticks_usec -= ticks_second * TICKS_SECOND;

	uint64_t ticks_ms = UtilityFunctions::floori(ticks_usec / TICKS_MS);
	ticks_usec -= ticks_ms * TICKS_MS;

	String timestamp = vformat("%d:%02d:%02d:%03d", ticks_hour, ticks_minute, ticks_second, ticks_ms);
	UtilityFunctions::print(vformat("%s YAML.register_class(%s)", timestamp, class_name));
	//#endif
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
	registry_mutex->lock();
	auto class_info = get_class_info(class_name);
	class_registry.erase(class_name);
	// Erase tag also
	if (class_info.tag != class_name && class_info.tag.length()) {
		class_registry.erase(class_info.tag);
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

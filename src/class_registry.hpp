/**
 * @file class_registry.hpp
 * @brief Defines the YAMLClassRegistry for custom class serialization.
 *
 * This file contains the YAMLClassRegistry class which manages the registration
 * of custom GDScript classes for YAML serialization and deserialization.
 * It provides a mechanism for mapping between class names and their
 * serialization/deserialization methods.
 */
#pragma once

#include <hashers.hpp>

#include <godot_cpp/classes/mutex.hpp>
#include <godot_cpp/classes/script.hpp>
#include <godot_cpp/variant/string.hpp>

#include <unordered_map>

namespace godot {

/**
 * @class YAMLClassRegistry
 * @brief Registry for custom class serialization/deserialization.
 *
 * The YAMLClassRegistry class provides facilities for registering and managing
 * custom GDScript classes that need YAML serialization support. It maintains
 * a mapping between class names and their serialization/deserialization methods.
 *
 * This enables users to define custom serialization logic for their own classes
 * through the YAML API, allowing complex objects to be properly serialized to
 * and deserialized from YAML.
 */
class YAMLClassRegistry {
public:
	/**
	 * @struct ClassInfo
	 * @brief Contains registration information for a custom class.
	 */
	struct ClassInfo {
		Ref<Script> script_class; ///< Reference to the script class
		String tag; ///< Custom YAML tag
		StringName serialize_method; ///< Instance method for serialization
		StringName deserialize_method; ///< Static method for deserialization
	};

	/**
	 * @brief Singleton instance
	 */
	static YAMLClassRegistry &get_singleton() {
		if (!singleton) {
			singleton = new YAMLClassRegistry();
		}
		return *singleton;
	}

	/**
	 * @brief Destroys the singleton instance and clears the registry.
	 */
	static void destroy_singleton() {
		if (singleton) {
			delete singleton;
			singleton = nullptr;
		}
	}

	/**
	 * @brief Registers a custom class for YAML serialization.
	 *
	 * @param p_class The script resource representing the class
	 * @param p_serialize The name of the instance method for serialization
	 * @param p_deserialize The name of the static method for deserialization
	 * @param p_tag The YAML tag to use for this class (optional)
	 */
	void register_class(Ref<Script> p_class, const Variant &p_serialize, const Variant &p_deserialize, const Variant &p_tag = Variant());

	/**
	 * @brief Unregisters a previously registered class.
	 *
	 * @param p_class The script resource representing the class
	 */
	void unregister_class(Ref<Script> p_class);

	/**
	 * @brief Checks if a class is registered.
	 *
	 * @param class_name The name of the class
	 * @return bool True if the class is registered
	 */
	bool has_class(const String &class_name);

	/**
	 * @brief Gets a list of all registered class names.
	 *
	 * @return PackedStringArray A list of class names
	 */
	PackedStringArray get_registered_classes();

	/**
	 * @brief Gets the registration information for a class.
	 *
	 * @param class_name The name of the class
	 * @return ClassInfo The registration information or an empty ClassInfo if not found
	 */
	ClassInfo get_class_info(const String &class_name);

	/**
	 * @brief Clears all registered classes from the registry.
	 */
	void clear() {
		if (registry_mutex.is_valid()) {
			registry_mutex->lock();
		}

#ifdef GODOT_YAML_DEBUG
		UtilityFunctions::print_verbose(vformat("YAML: Clearing class registry with %s classes", class_registry.size()));
#endif

		class_registry.clear();
		if (registry_mutex.is_valid()) {
			registry_mutex->unlock();
			registry_mutex.unref();
		}
	}

private:
	static YAMLClassRegistry *singleton;

	/**
	 * @brief Private constructor for singleton pattern.
	 */
	YAMLClassRegistry() { registry_mutex = Ref<Mutex>(memnew(Mutex)); }

	/**
	 * @brief Private destructor for singleton pattern.
	 */
	~YAMLClassRegistry() { clear(); }

	YAMLClassRegistry(YAMLClassRegistry const &); // Don't Implement
	void operator=(YAMLClassRegistry const &); // Don't implement

	/**
	 * @brief Gets the global class name for a script.
	 *
	 * @param p_class The script resource
	 * @return String The global class name or empty if not available
	 */
	String get_script_class(Ref<Script> p_class);

	/**
	 * @brief Mutex for thread-safe registry access.
	 */
	Ref<Mutex> registry_mutex;

	/**
	 * @brief Map of class names to registration information.
	 */
	std::unordered_map<String, ClassInfo, StringHasher, StringEqual> class_registry;
};

} // namespace godot

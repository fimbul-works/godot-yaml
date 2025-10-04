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

#include <godot_cpp/classes/script.hpp>
#include <godot_cpp/variant/string.hpp>

#include <mutex>
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
		StringName tag; ///< YAML tag
		StringName serialize_method; ///< Instance method for serialization
		StringName deserialize_method; ///< Static method for deserialization
	};

	/**
	 * @brief Registers a custom class for YAML serialization.
	 *
	 * @param p_class The script resource representing the class
	 * @param p_serialize The name of the instance method for serialization
	 * @param p_deserialize The name of the static method for deserialization
	 */
	static void register_class(
			Ref<Script> p_class, const Variant &p_serialize, const Variant &p_deserialize, const Variant &p_tag = "");

	/**
	 * @brief Unregisters a previously registered class.
	 *
	 * @param p_class The script resource representing the class
	 */
	static void unregister_class(Ref<Script> p_class);

	/**
	 * @brief Checks if a class is registered.
	 *
	 * @param class_name The name of the class
	 * @return bool True if the class is registered
	 */
	static bool has_class(const String &class_name);

	/**
	 * @brief Gets the registration information for a class.
	 *
	 * @param class_name The name of the class
	 * @return ClassInfo The registration information or an empty ClassInfo if not found
	 */
	static ClassInfo get_class_info(const String &class_name);

private:
	/**
	 * @brief Gets the global class name for a script.
	 *
	 * @param p_class The script resource
	 * @return String The global class name or empty if not available
	 */
	static String get_script_class(Ref<Script> p_class);

	/**
	 * @brief Mutex for thread-safe registry access.
	 */
	static std::mutex registry_mutex;

	/**
	 * @brief Map of class names to registration information.
	 */
	static std::unordered_map<String, ClassInfo, StringHasher, StringEqual> class_registry;
};

} // namespace godot

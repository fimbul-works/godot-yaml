/**
 * @file security.h
 * @brief Defines the YAMLSecurity class for resource loading security.
 *
 * This file contains the YAMLSecurity class which provides security features
 * for loading resources during YAML parsing. It enables path and type restrictions
 * to prevent loading potentially harmful resources.
 */

#ifndef YAML_SECURITY_H
#define YAML_SECURITY_H

#include "string_hash.h"
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace godot;

/**
 * @class YAMLSecurity
 * @brief Provides security features for resource loading in YAML.
 *
 * The YAMLSecurity class manages security settings for loading resources
 * during YAML parsing. It allows for restricting resource loading by:
 * - Path prefixes (e.g., only allow resources from "res://assets/")
 * - Resource types (e.g., block Script or GDExtension resources)
 *
 * It provides thread-safe validation through an immutable View class
 * and supports singleton access for global security settings.
 *
 * @extends RefCounted
 */
class YAMLSecurity : public RefCounted {
	GDCLASS(YAMLSecurity, RefCounted);

protected:
	/**
	 * @brief Binds methods to make them accessible from GDScript.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Constructor that initializes with default security settings.
	 *
	 * By default, Script and GDExtension types are blocked for security reasons.
	 */
	YAMLSecurity();

	/**
	 * @brief Destructor.
	 */
	~YAMLSecurity();

	/**
	 * @brief Gets the default singleton instance.
	 *
	 * @return YAMLSecurity* Pointer to the default instance
	 */
	static YAMLSecurity *get_default_instance();

	/**
	 * @brief Cleans up the default instance.
	 *
	 * Called during module cleanup to free resources.
	 */
	static void cleanup_default_instance();

	/**
	 * @brief Public API methods.
	 */

	/**
	 * @brief Allows resource loading from a specific path prefix.
	 *
	 * @param path_prefix The path prefix to allow
	 * @param type_names Optional array of allowed resource types for this path
	 */
	void allow_path(const String &path_prefix, const Array &type_names = Array());

	/**
	 * @brief Blocks a specific resource type from being loaded.
	 *
	 * @param type_name The resource type to block
	 */
	void block_type(const StringName &type_name);

	/**
	 * @brief Clears all path restrictions.
	 */
	void clear_path_restrictions();

	/**
	 * @brief Clears all type restrictions.
	 */
	void clear_type_restrictions();

	/**
	 * @brief Resets security settings to default values.
	 *
	 * Blocks Script and GDExtension types by default.
	 */
	void reset();

	/**
	 * @brief Converts the security object to a string representation.
	 *
	 * @return String String representation of the security object
	 */
	String _to_string() const;

	/**
	 * @class View
	 * @brief The immutable View class for thread-safe validation.
	 *
	 * This class provides an immutable snapshot of security settings
	 * for thread-safe validation during parsing.
	 */
	class View {
	public:
		/**
		 * @brief Default constructor for an empty view.
		 */
		View() {}

		/**
		 * @brief Constructor that takes a snapshot of security settings.
		 *
		 * @param allowed_paths_with_types Map of allowed paths with type restrictions
		 * @param blocked_types Set of globally blocked types
		 */
		View(
				const std::unordered_map<String, std::vector<StringName>, StringHasher> &allowed_paths_with_types,
				const std::unordered_set<StringName, StringNameHasher> &blocked_types);

		/**
		 * @brief Checks if a path is allowed (first step validation).
		 *
		 * @param path The resource path to check
		 * @return bool True if the path is allowed
		 */
		bool is_path_allowed(const String &path) const;

		/**
		 * @brief Checks if a resource is allowed (second step validation).
		 *
		 * @param path The resource path
		 * @param class_name The resource class name
		 * @return bool True if the resource is allowed
		 */
		bool is_resource_allowed(const String &path, const StringName &class_name) const;

		/**
		 * @brief Checks if a resource is allowed (object variant).
		 *
		 * @param path The resource path
		 * @param resource The resource object
		 * @return bool True if the resource is allowed
		 */
		bool is_resource_allowed(const String &path, const Object *resource) const;

	private:
		/**
		 * @brief Immutable copies of security settings.
		 */
		std::unordered_map<String, std::vector<StringName>, StringHasher> allowed_paths_with_types;
		std::unordered_set<StringName, StringNameHasher> blocked_types;
	};

	/**
	 * @brief Gets a View of the current security config.
	 *
	 * @return View An immutable view of current security settings
	 */
	View get_view() const;

	/**
	 * @brief Gets the default view (convenience method).
	 *
	 * @return View The default security view
	 */
	static View get_default_view();

private:
	/**
	 * @brief Paths with specific type restrictions.
	 *
	 * An empty vector means all non-blocked types are allowed.
	 */
	std::unordered_map<String, std::vector<StringName>, StringHasher> allowed_paths_with_types;

	/**
	 * @brief Blocked types (globally blocked regardless of path).
	 */
	std::unordered_set<StringName, StringNameHasher> blocked_types;

	/**
	 * @brief Mutex for thread-safety during configuration.
	 */
	mutable std::mutex mutex;

	/**
	 * @brief Singleton instance.
	 */
	static YAMLSecurity *default_instance;
};

#endif // YAML_SECURITY_H

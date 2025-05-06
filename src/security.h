/**
 * @file security.h
 * @brief Defines the PathPattern and YAMLSecurity classes for resource loading security.
 *
 * This file contains the PathPattern class for path matching and the YAMLSecurity
 * class which provides security features for loading resources during YAML parsing.
 * It enables path and type restrictions to prevent loading potentially harmful resources.
 */

#ifndef YAML_SECURITY_H
#define YAML_SECURITY_H

#include "string_hash.h" // Assuming this provides StringHasher and potentially std::hash<String>
#include <functional> // For std::hash and std::equal_to
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <mutex>
#include <string> // Required for std::hash<std::string> if String is not directly hashable by std::hash
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace godot;

// Forward declaration
class YAMLSecurity;

/**
 * @enum PathType
 * @brief Defines the type of path pattern for security rules.
 */
enum PathType {
	REGULAR, ///< Regular path prefix without wildcards
	SINGLE_WILDCARD, ///< Path with single segment wildcards
	RECURSIVE_WILDCARD ///< Path with recursive wildcards
};

/**
 * @class PathPattern
 * @brief Helper class for path pattern storage and matching.
 *
 * This class handles the parsing and matching of path patterns,
 * including support for wildcards.
 */
class PathPattern {
public:
	/**
	 * @brief Default constructor.
	 */
	PathPattern() :
			type(REGULAR), path("") {}

	/**
	 * @brief Constructor with path string.
	 *
	 * Automatically determines the pattern type based on the presence
	 * of wildcards in the path.
	 *
	 * @param p_path The path pattern string
	 */
	PathPattern(const String &p_path);

	/**
	 * @brief Checks if a path matches this pattern.
	 *
	 * @param p_path The path to check against this pattern
	 * @return bool True if the path matches the pattern
	 */
	bool matches(const String &p_path) const;

	/**
	 * @brief Gets the original path pattern string.
	 *
	 * @return const String& The pattern string
	 */
	const String &get_path() const { return path; }

	/**
	 * @brief Gets the pattern type.
	 *
	 * @return PathType The type of path pattern
	 */
	PathType get_type() const { return type; }

private:
	/**
	 * @brief Splits a path into segments.
	 *
	 * @param p_path The path to split
	 * @return std::vector<String> Vector of path segments
	 */
	static std::vector<String> split_path(const String &p_path);

	/**
	 * @brief Matches a path against a pattern using regular prefix matching.
	 *
	 * @param p_path The path to check
	 * @return bool True if the path matches
	 */
	bool match_regular(const String &p_path) const;

	/**
	 * @brief Matches a path against a pattern with single segment wildcards.
	 *
	 * @param p_path The path to check
	 * @return bool True if the path matches
	 */
	bool match_single_wildcard(const String &p_path) const;

	/**
	 * @brief Matches a path against a pattern with recursive wildcards.
	 *
	 * @param p_path The path to check
	 * @return bool True if the path matches
	 */
	bool match_recursive_wildcard(const String &p_path) const;

	PathType type; ///< The type of path pattern
	String path; ///< The original path pattern string
	std::vector<String> segments; ///< The pattern split into segments
};

// Hash and equality specializations for PathPattern
namespace std {
template <>
struct hash<PathPattern> {
	size_t operator()(const PathPattern &p) const {
		// Assuming godot::String has a suitable hash function or can be converted to std::string
		// If godot::String does not have a std::hash specialization, you might need to provide one
		// or use a custom hash function e.g. p.get_path().hash() if available.
		// For this example, we'll assume String can be hashed directly or via String::utf8 an_instance_method
		return std::hash<std::string>()(p.get_path().utf8().get_data());
	}
};

template <>
struct equal_to<PathPattern> {
	bool operator()(const PathPattern &a, const PathPattern &b) const {
		return a.get_path() == b.get_path();
	}
};
} //namespace std

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
 * Path wildcards are supported for flexible path matching:
 * - Single-level wildcards (using the "*" character)
 * - Multi-level wildcards (using the "**" pattern)
 *
 * See the README documentation for detailed examples of wildcard usage.
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
	 * The path prefix can include wildcards for flexible path matching.
	 * See the README documentation for detailed examples of wildcard usage.
	 *
	 * @param path_prefix The path prefix to allow, can include wildcards
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
		 * @param p_allowed_paths_with_types Map of path patterns with type restrictions
		 * @param p_blocked_types Set of globally blocked types
		 */
		View(
				const std::unordered_map<PathPattern, std::vector<StringName>, std::hash<PathPattern>, std::equal_to<PathPattern>> &p_allowed_paths_with_types,
				const std::unordered_set<StringName, StringNameHasher> &p_blocked_types);

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
		std::vector<std::pair<PathPattern, std::vector<StringName>>> allowed_paths_with_types;
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
	 * Maps path patterns to allowed type lists.
	 * An empty vector means all non-blocked types are allowed.
	 */
	std::unordered_map<PathPattern, std::vector<StringName>, std::hash<PathPattern>, std::equal_to<PathPattern>> allowed_paths_with_types;

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

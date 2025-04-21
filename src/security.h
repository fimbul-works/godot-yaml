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

class YAMLSecurity : public RefCounted {
	GDCLASS(YAMLSecurity, RefCounted);

protected:
	static void _bind_methods();

public:
	YAMLSecurity();
	~YAMLSecurity();

	// Static methods to get the default instance
	static YAMLSecurity *get_default_instance();
	static void cleanup_default_instance();

	// Public API
	void allow_path(const String &path_prefix, const Array &type_names = Array());
	void block_type(const StringName &type_name);
	void clear_path_restrictions();
	void clear_type_restrictions();
	void reset();

	// The immutable View class for thread-safe validation
	class View {
	public:
		View() {}

		// Constructor takes a snapshot of the security settings
		View(
				const std::unordered_map<String, std::vector<StringName>, StringHasher> &allowed_paths_with_types,
				const std::unordered_set<StringName, StringNameHasher> &blocked_types);

		// Check if a path is allowed (first step validation)
		bool is_path_allowed(const String &path) const;

		// Check if a resource is allowed (second step validation)
		bool is_resource_allowed(const String &path, const StringName &class_name) const;
		bool is_resource_allowed(const String &path, const Object *resource) const;

	private:
		// Immutable copies of security settings
		std::unordered_map<String, std::vector<StringName>, StringHasher> allowed_paths_with_types;
		std::unordered_set<StringName, StringNameHasher> blocked_types;
	};

	// Get a View of the current security config (needed for parser)
	View get_view() const;

	// Get the default view (for convenience)
	static View get_default_view();

private:
	// Paths with specific type restrictions
	// (empty vector means all non-blocked types allowed)
	std::unordered_map<String, std::vector<StringName>, StringHasher> allowed_paths_with_types;

	// Blocked types (globally blocked regardless of path)
	std::unordered_set<StringName, StringNameHasher> blocked_types;

	// Mutex for thread-safety during configuration
	mutable std::mutex mutex;

	// Singleton instance
	static YAMLSecurity *default_instance;
};

#endif // YAML_SECURITY_H

#include "security.h"
#include "yaml.h"

// Initialize static members
YAMLSecurity *YAMLSecurity::default_instance = nullptr;

YAMLSecurity *YAMLSecurity::get_default_instance() {
	if (!default_instance) {
		default_instance = memnew(YAMLSecurity());
	}

	return default_instance;
}

YAMLSecurity::YAMLSecurity() {
	// Start with empty containers
	allowed_paths_with_types.clear();
	blocked_types.clear();

	// Add default blocked types
	blocked_types.insert(StringName("Script"));
	blocked_types.insert(StringName("GDExtension"));
}

YAMLSecurity::~YAMLSecurity() {
}

void YAMLSecurity::cleanup_default_instance() {
	if (default_instance) {
		memdelete(default_instance);
		default_instance = nullptr;
	}
}

void YAMLSecurity::_bind_methods() {
	ClassDB::bind_method(D_METHOD("allow_path", "path_prefix", "type_names"), &YAMLSecurity::allow_path, DEFVAL(Array()));
	ClassDB::bind_method(D_METHOD("block_type", "type_name"), &YAMLSecurity::block_type);

	ClassDB::bind_method(D_METHOD("clear_path_restrictions"), &YAMLSecurity::clear_path_restrictions);
	ClassDB::bind_method(D_METHOD("clear_type_restrictions"), &YAMLSecurity::clear_type_restrictions);
	ClassDB::bind_method(D_METHOD("reset"), &YAMLSecurity::reset);
}

void YAMLSecurity::allow_path(const String &path_prefix, const Array &type_names) {
	std::lock_guard<std::mutex> lock(mutex);

	// Create or clear the entry for this path
	std::vector<StringName> &allowed_types = allowed_paths_with_types[path_prefix];
	allowed_types.clear();

	// Add all specified types
	for (int i = 0; i < type_names.size(); i++) {
		allowed_types.push_back(type_names[i]);
	}
}

void YAMLSecurity::block_type(const StringName &type_name) {
	std::lock_guard<std::mutex> lock(mutex);
	if (blocked_types.find(type_name) != blocked_types.end()) {
		return;
	}

	// Use bracket operator which is safer for insertion
	blocked_types.insert(type_name);
}

void YAMLSecurity::clear_path_restrictions() {
	std::lock_guard<std::mutex> lock(mutex);
	allowed_paths_with_types.clear();
}

void YAMLSecurity::clear_type_restrictions() {
	std::lock_guard<std::mutex> lock(mutex);
	blocked_types.clear();
}

void YAMLSecurity::reset() {
	std::lock_guard<std::mutex> lock(mutex);

	allowed_paths_with_types.clear();
	blocked_types.clear();

	blocked_types.insert(StringName("Script"));
	blocked_types.insert(StringName("GDExtension"));
}

YAMLSecurity::View YAMLSecurity::get_view() const {
	std::lock_guard<std::mutex> lock(mutex);
	return View(allowed_paths_with_types, blocked_types);
}

YAMLSecurity::View YAMLSecurity::get_default_view() {
	return get_default_instance()->get_view();
}

// View implementation
YAMLSecurity::View::View(
		const std::unordered_map<String, std::vector<StringName>, StringHasher> &allowed_paths_with_types,
		const std::unordered_set<StringName, StringNameHasher> &blocked_types) :
		allowed_paths_with_types(allowed_paths_with_types), blocked_types(blocked_types) {
}

bool YAMLSecurity::View::is_resource_allowed(const String &path, const StringName &class_name) const {
	// Check if type is globally blocked
	if (blocked_types.find(class_name) != blocked_types.end()) {
		return false;
	}

	// Check parent classes for block rules
	StringName parent_class = ClassDB::get_parent_class(class_name);
	while (parent_class != StringName()) {
		if (blocked_types.find(parent_class) != blocked_types.end()) {
			return false;
		}
		parent_class = ClassDB::get_parent_class(parent_class);
	}

	// If no paths are defined, all non-blocked types are allowed
	if (allowed_paths_with_types.empty()) {
		return true;
	}

	// Check if path matches any allowed path prefix
	for (const auto &entry : allowed_paths_with_types) {
		if (path.begins_with(entry.first)) {
			// Empty type list means allow all non-blocked types
			if (entry.second.empty()) {
				return true;
			}

			// Check if this type or any parent is in the allowed list
			for (const auto &allowed_type : entry.second) {
				// Check if class_name is the same as or inherits from allowed_type
				if (class_name == allowed_type || ClassDB::is_parent_class(class_name, allowed_type)) {
					return true;
				}
			}

			// Type not in whitelist for this path
			return false;
		}
	}

	// No matching path, deny
	return false;
}

bool YAMLSecurity::View::is_path_allowed(const String &path) const {
	// If no paths are defined, all paths are allowed
	if (allowed_paths_with_types.empty()) {
		return true;
	}

	// Check if path matches any allowed path prefix
	for (const auto &entry : allowed_paths_with_types) {
		if (path.begins_with(entry.first)) {
			return true;
		}
	}

	// No matching path, deny
	return false;
}

bool YAMLSecurity::View::is_resource_allowed(const String &path, const Object *resource) const {
	if (!resource) {
		return false;
	}
	return is_resource_allowed(path, resource->get_class());
}

#include "security.hpp"

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

YAMLSecurity::~YAMLSecurity() {}

void YAMLSecurity::cleanup_default_instance() {
	if (default_instance) {
		memdelete(default_instance);
		default_instance = nullptr;
	}
}

void YAMLSecurity::_bind_methods() {
	ClassDB::bind_method(
			D_METHOD("allow_path", "path_prefix", "type_names"), &YAMLSecurity::allow_path, DEFVAL(Array()));
	ClassDB::bind_method(D_METHOD("block_type", "type_name"), &YAMLSecurity::block_type);
	ClassDB::bind_method(D_METHOD("clear_path_restrictions"), &YAMLSecurity::clear_path_restrictions);
	ClassDB::bind_method(D_METHOD("clear_type_restrictions"), &YAMLSecurity::clear_type_restrictions);
	ClassDB::bind_method(D_METHOD("reset"), &YAMLSecurity::reset);
	BIND_VIRTUAL_METHOD(YAMLSecurity, _to_string);
}

void YAMLSecurity::allow_path(const String &path_prefix, const Array &type_names) {
	std::lock_guard<std::mutex> lock(mutex);
	PathPattern pattern(path_prefix);
	std::vector<StringName> &allowed_types = allowed_paths_with_types[pattern];
	allowed_types.clear();
	for (int i = 0; i < type_names.size(); i++) {
		allowed_types.push_back(type_names[i]);
	}
}

void YAMLSecurity::block_type(const StringName &type_name) {
	std::lock_guard<std::mutex> lock(mutex);
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

PathPattern::PathPattern(const String &p_path) : path(p_path) {
	if (p_path.find("**") != -1) {
		type = RECURSIVE_WILDCARD;
	} else if (p_path.find("*") != -1) {
		type = SINGLE_WILDCARD;
	} else {
		type = REGULAR;
	}
	if (type != REGULAR) {
		segments = split_path(p_path);
	}
}

bool PathPattern::matches(const String &p_path) const {
	switch (type) {
		case REGULAR:
			return match_regular(p_path);
		case SINGLE_WILDCARD:
			return match_single_wildcard(p_path);
		case RECURSIVE_WILDCARD:
			return match_recursive_wildcard(p_path);
		default:
			return false;
	}
}

bool PathPattern::match_regular(const String &p_path) const {
	return p_path.begins_with(path);
}

bool PathPattern::match_single_wildcard(const String &p_path) const {
	std::vector<String> path_segments = split_path(p_path);
	if (path_segments.size() < segments.size()) {
		return false;
	}
	for (size_t i = 0; i < segments.size(); i++) {
		const String &pattern_segment = segments[i];
		const String &path_segment = path_segments[i];
		if (pattern_segment == "*") {
			continue;
		} else if (pattern_segment != path_segment) {
			return false;
		}
	}
	return true;
}

bool PathPattern::match_recursive_wildcard(const String &p_path) const {
	std::vector<String> path_segments = split_path(p_path);
	std::vector<std::vector<bool>> dp(segments.size() + 1, std::vector<bool>(path_segments.size() + 1, false));
	dp[0][0] = true;

	for (size_t i = 1; i <= segments.size(); i++) {
		if (segments[i - 1] == "**") {
			dp[i][0] = dp[i - 1][0];
			for (size_t j = 1; j <= path_segments.size(); j++) {
				dp[i][j] = dp[i - 1][j] || dp[i][j - 1];
			}
		} else if (segments[i - 1] == "*") {
			for (size_t j = 1; j <= path_segments.size(); j++) {
				if (dp[i - 1][j - 1]) {
					dp[i][j] = true;
				}
			}
		} else {
			for (size_t j = 1; j <= path_segments.size(); j++) {
				if (dp[i - 1][j - 1] && (segments[i - 1] == path_segments[j - 1])) {
					dp[i][j] = true;
				}
			}
		}
	}

	for (size_t j = 0; j <= path_segments.size(); j++) { // Adjusted loop to start from 0 for ** matching empty prefix
		if (dp[segments.size()][j]) {
			if (j == path_segments.size()) {
				return true;
			} else if (!segments.empty() && segments.back() == "**") {
				// If the pattern matches up to 'j' and the pattern ends with '**',
				// then '**' can consume the rest of path_segments.
				return true;
			}
		}
	}
	return false;
}

std::vector<String> PathPattern::split_path(const String &p_path) {
	std::vector<String> result;
	String current;
	for (int i = 0; i < p_path.length(); i++) {
		char32_t c = p_path[i];
		if (c == '/') {
			if (!current.is_empty()) {
				result.push_back(current);
				current = "";
			}
		} else {
			current += c;
		}
	}
	if (!current.is_empty()) {
		result.push_back(current);
	}
	return result;
}

YAMLSecurity::View::View(const std::unordered_map<PathPattern, std::vector<StringName>, std::hash<PathPattern>,
								 std::equal_to<PathPattern>> &p_allowed_paths_with_types,
		const std::unordered_set<StringName, StringNameHasher> &p_blocked_types) :
		blocked_types(p_blocked_types) {
	for (const auto &entry : p_allowed_paths_with_types) {
		allowed_paths_with_types.push_back({ entry.first, entry.second });
	}
}

bool YAMLSecurity::View::is_path_allowed(const String &path) const {
	if (allowed_paths_with_types.empty()) {
		return true;
	}
	for (const auto &entry : allowed_paths_with_types) {
		if (entry.first.matches(path)) {
			return true;
		}
	}
	return false;
}

bool YAMLSecurity::View::is_resource_allowed(const String &path, const StringName &class_name) const {
	if (blocked_types.find(class_name) != blocked_types.end()) {
		return false;
	}
	StringName parent_class = ClassDB::get_parent_class(class_name);
	while (parent_class != StringName()) {
		if (blocked_types.find(parent_class) != blocked_types.end()) {
			return false;
		}
		parent_class = ClassDB::get_parent_class(parent_class);
	}

	if (allowed_paths_with_types.empty()) {
		return true;
	}

	for (const auto &entry : allowed_paths_with_types) {
		if (entry.first.matches(path)) {
			if (entry.second.empty()) {
				return true;
			}
			for (const auto &allowed_type : entry.second) {
				if (class_name == allowed_type || ClassDB::is_parent_class(class_name, allowed_type)) {
					return true;
				}
			}
			return false;
		}
	}
	return false;
}

bool YAMLSecurity::View::is_resource_allowed(const String &path, const Object *resource) const {
	if (!resource) {
		return false;
	}
	return is_resource_allowed(path, resource->get_class());
}

String YAMLSecurity::_to_string() const {
	String result_paths;
	bool first_path = true;
	if (allowed_paths_with_types.empty()) {
		result_paths = "*";
	} else {
		for (const auto &pair : allowed_paths_with_types) {
			if (!first_path) {
				result_paths += "; ";
			}
			// NOTE: Cast size_t to int64_t for ARM64 macOS Variant compatibility
			result_paths += vformat("%s (types: %d)", pair.first.get_path(), static_cast<int64_t>(pair.second.size()));
			first_path = false;
		}
	}

	String result_blocked_types;
	bool first_type = true;
	for (const StringName &type_name : blocked_types) {
		if (!first_type) {
			result_blocked_types += ",";
		}
		result_blocked_types += String(type_name);
		first_type = false;
	}
	return vformat("YAMLSecurity(Paths: [%s], BlockedTypes: [%s])", result_paths, result_blocked_types);
}

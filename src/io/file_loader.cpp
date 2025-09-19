#include "file_loader.hpp"

using namespace godot;

Ref<YAMLResult> YAML::FileLoader::load_file(const String &path, const Ref<YAMLSecurity> security, const bool detect_style) {
	YAMLSecurity::View security_view = security.is_valid() ? security->get_view() : YAMLSecurity::get_default_view();

	String content;
	Ref<YAMLResult> read_result = read_file_content(path, content);
	if (read_result->has_error()) {
		return read_result;
	}

	// Create a path tracking set for this load operation
	std::unordered_set<String, StringHasher, StringEqual> loading_yaml_paths;

	// Parse the content using a Parser instance
	Parser parser;
	return parser.parse(content, security_view, detect_style);
}

Ref<YAMLResult> YAML::FileLoader::parser_load_file(const String &path, const YAMLSecurity::View &security_view, std::unordered_set<String, StringHasher, StringEqual> *loading_yaml_paths) {
	String content;
	Ref<YAMLResult> read_result = read_file_content(path, content);
	if (read_result->has_error()) {
		return read_result;
	}

	// Check for cyclical references for YAML files
	if (path.to_lower().ends_with(".yaml") || path.to_lower().ends_with(".yml")) {
		if (loading_yaml_paths && loading_yaml_paths->find(path) != loading_yaml_paths->end()) {
			return YAMLResult::error(vformat("Cyclical YAML reference detected: %s", path));
		}
	}

	// Parse the content using a Parser instance with shared path tracking
	Parser parser(loading_yaml_paths);
	return parser.parse(content, security_view, false);
}

Ref<YAMLResult> YAML::FileLoader::read_file_content(const String &path, String &content) {
	if (!FileAccess::file_exists(path)) {
		return YAMLResult::error("File not found '" + path + "': " + UtilityFunctions::error_string(ERR_FILE_NOT_FOUND));
	}

	// Open file for reading
	Ref<FileAccess> file = FileAccess::open(path, FileAccess::READ);
	if (!file.is_valid()) {
		return YAMLResult::error("Failed to read '" + path + "': " + UtilityFunctions::error_string(ERR_FILE_BAD_PATH));
	}

	Error err = file->get_error();
	if (err != OK) {
		return YAMLResult::error("Failed to read '" + path + "': " + UtilityFunctions::error_string((int)err));
	}

	// Read the file content and check for errors
	content = file->get_as_text();
	err = file->get_error();
	file->close();

	if (err != OK) {
		return YAMLResult::error("Failed to read '" + path + "': " + UtilityFunctions::error_string((int)err));
	}

	return YAMLResult::success(Variant());
}

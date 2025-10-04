#include "file_validator.hpp"

using namespace godot;

YAML::FileValidator::FileValidator() {}

Ref<YAMLResult> YAML::FileValidator::validate_file(const String &path) {
	if (!FileAccess::file_exists(path)) {
		return YAMLResult::error(
				"File not found '" + path + "': " + UtilityFunctions::error_string(ERR_FILE_NOT_FOUND));
	}

	// Open file for reading
	Ref<FileAccess> file = FileAccess::open(path, FileAccess::READ);

	if (!file.is_valid()) {
		return YAMLResult::error(
				"Failed to validate '" + path + "': " + UtilityFunctions::error_string(ERR_FILE_BAD_PATH));
	}

	Error err = file->get_error();
	if (err != OK) {
		return YAMLResult::error("Failed to validate '" + path + "': " + UtilityFunctions::error_string((int)err));
	}

	// Read the file content and check for errors
	String content = file->get_as_text();
	err = file->get_error();
	file->close();

	if (err != OK) {
		return YAMLResult::error("Failed to validate '" + path + "': " + UtilityFunctions::error_string((int)err));
	}

	return YAML::validate(content);
}

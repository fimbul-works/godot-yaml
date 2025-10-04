#include "file_saver.hpp"

using namespace godot;

Ref<YAMLResult> YAML::FileSaver::save_file(const Variant &data, const String &path, const Ref<YAMLStyle> &style) {
	// First serialize the data to YAML
	Ref<YAMLResult> stringify_result = stringify(data, style);
	if (stringify_result->has_error()) {
		return stringify_result;
	}

	String yaml_content = stringify_result->get_data();
	return write_file_content(yaml_content, path);
}

Ref<YAMLResult> YAML::FileSaver::stringify(const Variant &data, const Ref<YAMLStyle> &style) {
	YAMLStyle::View style_view = style.is_valid() ? YAMLStyle::View::create_view(style) : YAMLStyle::View();

	Emitter emitter;
	return emitter.emit(data, style_view);
}

Ref<YAMLResult> YAML::FileSaver::write_file_content(const String &yaml_content, const String &path) {
	// Open file for writing
	Ref<FileAccess> file = FileAccess::open(path, FileAccess::WRITE);
	if (!file.is_valid()) {
		return YAMLResult::error(
				"Failed to write '" + path + "': " + UtilityFunctions::error_string(ERR_FILE_BAD_PATH));
	}

	Error err = file->get_error();
	if (err != OK) {
		return YAMLResult::error("Failed to write '" + path + "': " + UtilityFunctions::error_string((int)err));
	}

	// Write the file content and check for errors
	file->store_string(yaml_content);
	err = file->get_error();
	file->close();

	if (err != OK) {
		return YAMLResult::error("Failed to write '" + path + "': " + UtilityFunctions::error_string((int)err));
	}

	// Return success with the YAML content
	return YAMLResult::success(yaml_content);
}

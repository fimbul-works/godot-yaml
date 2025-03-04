class_name YAMLLoader

static var last_error: Variant = null

static func load_string(yaml_str: String) -> Variant:
	var result := YAML.parse(yaml_str)
	if result.has_error():
		last_error = "%s at %d:%d" % [result.get_error_message(), result.get_error_line(), result.get_error_column()]
		push_error("YAML parse error: " + last_error)
		return null
	last_error = null
	return result.get_data()

static func load_file(path: String) -> Variant:
	if not FileAccess.file_exists(path):
		push_error("File not found: " + path)
		return null

	var file := FileAccess.open(path, FileAccess.READ)
	if not file:
		push_error("Could not open file: " + path)
		return null

	var content := file.get_as_text()
	return load_string(content)

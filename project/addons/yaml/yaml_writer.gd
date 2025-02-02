class_name YAMLWriter

static func save_string(data: Variant, style: YAMLStyle = null) -> String:
	var result := YAML.stringify(data, style)
	if result.has_error():
		push_error("YAML stringify error: " + result.get_error_message())
		return ""
	return result.get_data()

static func save_file(data: Variant, path: String, style: YAMLStyle = null) -> bool:
	var yaml := save_string(data, style)
	if yaml.is_empty():
		return false

	var file := FileAccess.open(path, FileAccess.WRITE)
	if not file:
		push_error("Could not open file for writing: " + path)
		return false

	file.store_string(yaml)
	return true

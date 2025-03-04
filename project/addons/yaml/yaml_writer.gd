class_name YAMLWriter

static var last_error: Variant = null

static func save_string(data: Variant, style: YAMLStyle = null) -> String:
	var result := YAML.stringify(data, style)
	if result.has_error():
		last_error = "%s at %d:%d" % [result.get_error_message(), result.get_error_line(), result.get_error_column()]
		push_error("YAML stringify error: " + last_error)
		return ""
	last_error = null
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

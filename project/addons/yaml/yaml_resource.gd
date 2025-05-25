@tool
@icon("res://addons/yaml/icon.svg")

class_name YAMLResource extends Resource
# This is a lightweight wrapper around YAML text files
# It doesn't process the YAML content, just holds the raw text

var text_content: String = ""
var file_path: String = ""

func _init(path: String = "", content: String = "") -> void:
	file_path = path
	text_content = content
	if not path.is_empty():
		resource_path = path

func get_text() -> String:
	return text_content

func set_text(new_text: String) -> void:
	text_content = new_text
	emit_changed()

func get_file_path() -> String:
	return file_path

func load_from_file() -> Error:
	if file_path.is_empty():
		return ERR_FILE_NOT_FOUND

	var file := FileAccess.open(file_path, FileAccess.READ)
	if not file:
		return FileAccess.get_open_error()

	text_content = file.get_as_text()
	file.close()
	return OK

func save_to_file() -> Error:
	if file_path.is_empty():
		return ERR_FILE_NOT_FOUND

	var file := FileAccess.open(file_path, FileAccess.WRITE)
	if not file:
		return FileAccess.get_open_error()

	file.store_string(text_content)
	file.close()
	return OK

func serialize() -> Variant:
	push_error("Child of YAMLResource needs to override serialize")
	return null

static func deserialize(data: Variant) -> Variant:
	push_error("Child of YAMLResource needs to override deserialize")
	return null

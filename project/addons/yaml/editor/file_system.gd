@tool
class_name YAMLFileSystem extends Node

signal file_opened(path)
signal file_saved(path)
signal file_updated(path)
signal file_closed(path)
signal file_renamed(old_path, new_path)

# Reference to editor interface for filesystem operations
var editor_interface: EditorInterface

# Singleton pattern
static var _instance: YAMLFileSystem
static func get_singleton() -> YAMLFileSystem:
	if not _instance:
		_instance = YAMLFileSystem.new()
		Engine.get_main_loop().root.call_deferred("add_child", _instance)
	return _instance

func _init() -> void:
	if _instance != null:
		push_error("YAMLFileSystem singleton already exists")
		return
	_instance = self
	# Mark as persistent so it doesn't get destroyed on scene changes
	process_mode = Node.PROCESS_MODE_ALWAYS

# Set the editor interface reference
func set_editor_interface(p_editor_interface: EditorInterface) -> void:
	editor_interface = p_editor_interface

# File operations with signals
func save_file(path: String, content: String) -> Error:
	var was_new_file = not file_exists(path)

	var file := FileAccess.open(path, FileAccess.WRITE)
	if not file:
		return FileAccess.get_open_error()

	file.store_string(content)
	file_saved.emit(path)
	file_updated.emit(path)

	# If this was a new file, notify Godot's filesystem
	if was_new_file and editor_interface:
		call_deferred("_refresh_filesystem", path)

	return OK

func read_file(path: String) -> Variant:
	var file := FileAccess.open(path, FileAccess.READ)
	if not file:
		return FileAccess.get_open_error()

	return file.get_as_text()

# Check if a file exists
func file_exists(path: String) -> bool:
	return FileAccess.file_exists(path)

# Utility to check if a path is a YAML file
func is_yaml_file(path: String) -> bool:
	return path.get_extension().to_lower() in ["yaml", "yml"]

# For external updates, allow code to manually trigger the signal
func notify_file_updated(path: String) -> void:
	file_updated.emit(path)

# Called when a file is opened in the editor
func notify_file_opened(path: String) -> void:
	file_opened.emit(path)

# Called when a file is closed in the editor
func notify_file_closed(path: String) -> void:
	file_closed.emit(path)

# Called when a file is renamed (by the filesystem or editor)
func notify_file_renamed(old_path: String, new_path: String) -> void:
	file_renamed.emit(old_path, new_path)

# Find a file by name in the filesystem
func find_file_in_filesystem(dir: EditorFileSystemDirectory, filename: String) -> String:
	# Check files in current directory
	for i in range(dir.get_file_count()):
		var file_path := dir.get_file_path(i)
		if file_path.get_file() == filename:
			return file_path

	# Recursively check subdirectories
	for i in range(dir.get_subdir_count()):
		var subdir := dir.get_subdir(i)
		var result := find_file_in_filesystem(subdir, filename)
		if not result.is_empty():
			return result

	return ""

func _refresh_filesystem(path: String) -> void:
	if editor_interface:
		editor_interface.get_resource_filesystem().update_file(path)

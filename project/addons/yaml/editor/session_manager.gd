@tool
class_name YAMLSessionManager
extends Node

const CONFIG_PATH = "res://.godot/yaml_editor_session.cfg"
const CONFIG_SECTION = "yaml_editor"
const CONFIG_KEY_OPEN_FILES = "open_files"
const CONFIG_KEY_SPLIT_OFFSET = "split_offset"  # New key for split position

var file_manager: YAMLFileManager
var config: ConfigFile
var autosave_timer: Timer
var resizable_container: HSplitContainer

func _ready() -> void:
	config = ConfigFile.new()

	# Setup autosave timer
	autosave_timer = Timer.new()
	autosave_timer.wait_time = 10.0  # Save session every 10 seconds
	autosave_timer.one_shot = false
	autosave_timer.autostart = true
	autosave_timer.timeout.connect(_on_autosave_timer_timeout)
	add_child(autosave_timer)

func setup(p_file_manager: YAMLFileManager, p_resizable_container: HSplitContainer) -> void:
	file_manager = p_file_manager
	resizable_container = p_resizable_container

	# Connect to file manager signals
	file_manager.file_opened.connect(_on_session_changed)
	file_manager.file_closed.connect(_on_session_changed)
	file_manager.current_file_changed.connect(_on_session_changed)
	resizable_container.dragged.connect(_on_split_dragged)

func _on_split_dragged(_offset: int) -> void:
	# The split position has changed, save the session
	_on_session_changed()

func save_session() -> void:
	# Don't save anything if we have no files
	if not is_instance_valid(file_manager):
		return

	var open_files = file_manager.get_open_files()

	# Create array of persistent file paths (skip untitled files)
	var persistent_files = []
	for path in open_files:
		if not path.begins_with("untitled"):
			persistent_files.append(path)

	# Save to config file
	config.set_value(CONFIG_SECTION, CONFIG_KEY_OPEN_FILES, persistent_files)
	config.set_value(CONFIG_SECTION, "current_file", file_manager.get_current_file_path() if not file_manager.get_current_file_path().begins_with("untitled") else "")

	# Save the split offset
	if is_instance_valid(resizable_container):
		config.set_value(CONFIG_SECTION, CONFIG_KEY_SPLIT_OFFSET, resizable_container.split_offset)

	var error = config.save(CONFIG_PATH)
	if error != OK:
		printerr("Failed to save YAML editor session: ", error)

func load_session() -> void:
	var error = config.load(CONFIG_PATH)
	if error != OK:
		# No saved session or error loading it
		if error != ERR_FILE_NOT_FOUND:
			printerr("Failed to load YAML editor session: ", error)
		return

	# Get saved file paths
	var file_paths = config.get_value(CONFIG_SECTION, CONFIG_KEY_OPEN_FILES, [])

	# Open each file
	for path in file_paths:
		if FileAccess.file_exists(path):
			file_manager.open_file(path)

	# Set current file
	var last_current = config.get_value(CONFIG_SECTION, "current_file", "")
	if not last_current.is_empty() and file_manager.has_file_open(last_current):
		file_manager.current_file_path = last_current
		file_manager.load_current_file_content()
		file_manager.current_file_changed.emit(last_current)

	# Restore split offset (deferred to ensure UI is ready)
	call_deferred("_restore_split_offset")

func _restore_split_offset() -> void:
	if is_instance_valid(resizable_container):
		var saved_offset = config.get_value(CONFIG_SECTION, CONFIG_KEY_SPLIT_OFFSET, resizable_container.split_offset)
		resizable_container.split_offset = saved_offset

func _on_session_changed(_path = "") -> void:
	# Set a short timer to prevent saving too frequently during batch operations
	autosave_timer.start()

func _on_autosave_timer_timeout() -> void:
	save_session()

@tool
class_name YAMLSessionManager
extends Node

const CONFIG_PATH = "user://yaml_editor_session.cfg"
const CONFIG_SECTION = "yaml_editor"
const CONFIG_KEY_OPEN_FILES = "open_files"

var file_manager: YAMLFileManager
var config: ConfigFile
var autosave_timer: Timer

func _ready() -> void:
	config = ConfigFile.new()

	# Setup autosave timer
	autosave_timer = Timer.new()
	autosave_timer.wait_time = 10.0  # Save session every 10 seconds
	autosave_timer.one_shot = false
	autosave_timer.autostart = true
	autosave_timer.timeout.connect(_on_autosave_timer_timeout)
	add_child(autosave_timer)

func setup(p_file_manager: YAMLFileManager) -> void:
	file_manager = p_file_manager

	# Connect to file manager signals
	file_manager.file_opened.connect(_on_session_changed)
	file_manager.file_closed.connect(_on_session_changed)
	file_manager.current_file_changed.connect(_on_session_changed)

func save_session() -> void:
	# Don't save anything if we have no files
	if not is_instance_valid(file_manager):
		return

	var open_files = file_manager.get_open_files()
	if open_files.is_empty():
		return

	# Create array of persistent file paths (skip untitled files)
	var persistent_files = []
	for path in open_files:
		if not path.begins_with("untitled"):
			persistent_files.append(path)

	# Save to config file
	config.set_value(CONFIG_SECTION, CONFIG_KEY_OPEN_FILES, persistent_files)
	config.set_value(CONFIG_SECTION, "current_file",
					 file_manager.get_current_file_path() if not file_manager.get_current_file_path().begins_with("untitled") else "")

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

func _on_session_changed(_path = "") -> void:
	# Set a short timer to prevent saving too frequently during batch operations
	autosave_timer.start()

func _on_autosave_timer_timeout() -> void:
	save_session()

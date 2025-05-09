@tool
class_name YAMLEditorSessionManager extends Node

const CONFIG_PATH := "res://.godot/yaml_editor_session.cfg"
const CONFIG_SECTION := "yaml_editor"
const CONFIG_KEY_OPEN_FILES := "open_files"
const CONFIG_KEY_SPLIT_OFFSET := "split_offset"
const CONFIG_KEY_CURRENT_FILE := "current_file"

var file_manager: YAMLEditorDocumentManager
var file_system: YAMLFileSystem
var config: ConfigFile
var autosave_timer: Timer
var resizable_container: HSplitContainer

func _ready() -> void:
	file_system = YAMLFileSystem.get_singleton()

	config = ConfigFile.new()

	# Setup autosave timer
	autosave_timer = Timer.new()
	add_child(autosave_timer)
	autosave_timer.wait_time = 10.0  # Save session every 10 seconds
	autosave_timer.one_shot = false
	autosave_timer.autostart = true
	autosave_timer.timeout.connect(_on_autosave_timer_timeout)

func setup(p_file_manager: YAMLEditorDocumentManager, p_resizable_container: HSplitContainer) -> void:
	file_manager = p_file_manager
	resizable_container = p_resizable_container

	# Connect to signals
	file_manager.document_changed.connect(_on_session_changed)
	file_manager.document_created.connect(_on_session_changed)
	file_manager.document_closed.connect(_on_session_changed)
	resizable_container.dragged.connect(_on_split_dragged)

func _on_split_dragged(_offset: int) -> void:
	# The split position has changed, save the session
	_on_session_changed()

func save_session() -> void:
	# Don't save anything if we have no files
	if not is_instance_valid(file_manager):
		return

	var documents: Array = file_manager.get_open_documents()

	# Create array of persistent file paths (skip untitled files)
	var persistent_files: PackedStringArray = []
	for document in documents:
		if not document.is_untitled():
			persistent_files.append(document.path)

	# Get current file path
	var current_path := ""
	var current_document := file_manager.get_current_document()
	if current_document and not current_document.is_untitled():
		current_path = current_document.path

	# Save to config file
	config.set_value(CONFIG_SECTION, CONFIG_KEY_OPEN_FILES, persistent_files)
	config.set_value(CONFIG_SECTION, CONFIG_KEY_CURRENT_FILE, current_path)

	# Save the split offset
	if is_instance_valid(resizable_container):
		config.set_value(CONFIG_SECTION, CONFIG_KEY_SPLIT_OFFSET, resizable_container.split_offset)

	var error := config.save(CONFIG_PATH)
	if error != OK:
		push_error("Failed to save YAML editor session: %s" % error_string(error))

func load_session() -> void:
	var error := config.load(CONFIG_PATH)
	if error != OK:
		# No saved session or error loading it
		if error != ERR_FILE_NOT_FOUND:
			push_error("Failed to load YAML editor session: ", error_string(error))
		return

	# Get saved file paths
	var file_paths: PackedStringArray = config.get_value(CONFIG_SECTION, CONFIG_KEY_OPEN_FILES, [])

	# Open each file
	for path in file_paths:
		if file_system.file_exists(path):
			file_manager.open_file(path)

	# Set current file
	var last_current: String = config.get_value(CONFIG_SECTION, CONFIG_KEY_CURRENT_FILE, "")
	if not last_current.is_empty() and file_manager.has_document(last_current):
		var document := file_manager.get_document(last_current)
		file_manager.set_current_document(document)

	# Restore split offset (deferred to ensure UI is ready)
	call_deferred("_restore_split_offset")

func _restore_split_offset() -> void:
	if is_instance_valid(resizable_container):
		var saved_offset: int = config.get_value(CONFIG_SECTION, CONFIG_KEY_SPLIT_OFFSET, resizable_container.split_offset)
		resizable_container.split_offset = saved_offset

func _on_session_changed(_document = null) -> void:
	# Set a short timer to prevent saving too frequently during batch operations
	autosave_timer.start()

func _on_autosave_timer_timeout() -> void:
	save_session()

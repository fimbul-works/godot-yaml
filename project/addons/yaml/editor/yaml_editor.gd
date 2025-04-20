@tool
extends Control

# Components
var file_manager: YAMLFileManager
var history_manager: YAMLHistoryManager
var session_manager: YAMLSessionManager
var validator: YAMLValidator

# Editor reference
var editor: EditorInterface

# UI references
@onready var menu_bar: YAMLMenuBar = %MenuBar
@onready var file_menu := %File
@onready var edit_menu := %Edit
@onready var search_menu := %Search
@onready var filter_input := %Filter

@onready var file_list := %FileList
@onready var code_edit := %YAMLCodeEdit
@onready var status_label := %StatusLabel
@onready var line_col_label := %LineColLabel
@onready var resizable_container := %ResizableContainer

func _ready() -> void:
	# Initialize components
	file_manager = YAMLFileManager.new()
	add_child(file_manager)

	history_manager = YAMLHistoryManager.new()
	add_child(history_manager)

	session_manager = YAMLSessionManager.new()
	add_child(session_manager)

	validator = YAMLValidator.new()
	add_child(validator)

	# Wait for UI to be ready
	await get_tree().process_frame

	filter_input.right_icon = get_theme_icon("Search", "EditorIcons")

	# Set up components
	file_manager.setup(file_list, code_edit, status_label)
	history_manager.setup(file_manager)
	session_manager.setup(file_manager, resizable_container)
	validator.setup(code_edit, status_label)

	# Connect toolbar signals
	menu_bar.new_file.connect(_on_new_button_pressed)
	menu_bar.open_file.connect(_on_open_button_pressed)
	menu_bar.save_requested.connect(_on_save_button_pressed)
	menu_bar.save_as_requested.connect(_on_save_as_button_pressed)
	menu_bar.undo_requested.connect(_on_undo_requested)
	menu_bar.redo_requested.connect(_on_redo_requested)

	# Connect code editor signals
	code_edit.snapshot_requested.connect(_on_snapshot_requested)
	code_edit.save_requested.connect(_on_save_button_pressed)
	code_edit.close_requested.connect(_on_close_current_file)
	code_edit.undo_requested.connect(_on_undo_requested)
	code_edit.redo_requested.connect(_on_redo_requested)
	code_edit.caret_changed.connect(_on_caret_changed)

	# Connect history manager signals for proper UI updating
	history_manager.undo_performed.connect(_on_undo_redo_performed)
	history_manager.redo_performed.connect(_on_undo_redo_performed)

	# Connect validation signals
	validator.validation_completed.connect(_on_validation_completed)

	# Share editor interface with components via tree metadata
	if editor:
		get_tree().set_meta("editor_interface", editor)

	# Load previous session
	session_manager.load_session()

func _on_new_button_pressed() -> void:
	file_manager.new_file()

func _on_open_button_pressed() -> void:
	var file_dialog = EditorFileDialog.new()
	file_dialog.file_mode = EditorFileDialog.FILE_MODE_OPEN_FILE
	file_dialog.access = EditorFileDialog.ACCESS_FILESYSTEM
	file_dialog.add_filter("*.yaml;YAML Files")
	file_dialog.add_filter("*.yml;YML Files")
	file_dialog.title = "Open YAML File"

	file_dialog.file_selected.connect(
		func(path):
			file_manager.open_file(path)
			file_dialog.queue_free()
	)
	file_dialog.canceled.connect(func(): file_dialog.queue_free())

	add_child(file_dialog)
	file_dialog.popup_centered_ratio(0.7)

func _on_save_button_pressed() -> void:
	if file_manager.get_current_file_path().begins_with("untitled"):
		_on_save_as_button_pressed()
	else:
		file_manager.save_current_file()

func _on_save_as_button_pressed() -> void:
	var file_dialog = EditorFileDialog.new()
	file_dialog.file_mode = EditorFileDialog.FILE_MODE_SAVE_FILE
	file_dialog.access = EditorFileDialog.ACCESS_FILESYSTEM
	file_dialog.add_filter("*.yaml;YAML Files")
	file_dialog.add_filter("*.yml;YML Files")
	file_dialog.title = "Save YAML File As"

	file_dialog.file_selected.connect(
		func(path):
			file_manager.save_file_as(path)
			file_dialog.queue_free()
	)
	file_dialog.canceled.connect(func(): file_dialog.queue_free())

	add_child(file_dialog)
	file_dialog.popup_centered_ratio(0.7)

func _on_validation_button_pressed() -> void:
	validator.validate_async(code_edit.text)

func _on_snapshot_requested() -> void:
	history_manager.take_snapshot()

func _on_close_current_file() -> void:
	file_manager.close_file(file_manager.get_current_file_path())

func _on_undo_requested() -> void:
	history_manager.perform_undo()

func _on_redo_requested() -> void:
	history_manager.perform_redo()

func _on_caret_changed() -> void:
	line_col_label.text = code_edit.get_current_line_col_info()

func _on_validation_completed(result) -> void:
	# Validation is handled by the validator component
	pass

func _has_unsaved_changes() -> bool:
	return file_manager.has_unsaved_changes()

func get_open_files() -> Array:
	return file_manager.get_open_files()

func handle_filesystem_change() -> void:
	file_manager.handle_filesystem_change()

func _notification(what):
	if what == NOTIFICATION_WM_CLOSE_REQUEST:
		# Save session when editor is closing
		session_manager.save_session()

func _on_undo_redo_performed(path: String) -> void:
	if is_instance_valid(line_col_label) and is_instance_valid(code_edit):
		# Use call_deferred to ensure this happens after the editor state is fully updated
		call_deferred("_update_line_col_label")

func _update_line_col_label() -> void:
	# Allow one frame to pass to ensure the UI is updated
	await get_tree().process_frame
	line_col_label.text = code_edit.get_current_line_col_info()

	# Ensure the cursor is visible
	code_edit.center_viewport_to_caret()

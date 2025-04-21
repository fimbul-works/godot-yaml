@tool
class_name YAMLEditor extends Control

# Components
var file_manager: YAMLFileManager
var validator: YAMLValidator
var session_manager: YAMLSessionManager

# File system singleton
var file_system: YAMLFileSystem

# Editor reference
var editor: EditorInterface

# UI references
@export var menu_bar: YAMLMenuBar
@export var file_list: YAMLFileList
@export var code_edit: YAMLCodeEditor
@export var status_panel: YAMLEditorStatusPanel
@export var resizable_container: HSplitContainer

func _ready() -> void:
	# Get reference to file system singleton first
	file_system = YAMLFileSystem.get_singleton()

	# Initialize components
	file_manager = YAMLFileManager.new()
	add_child(file_manager)

	validator = YAMLValidator.new()
	add_child(validator)

	session_manager = YAMLSessionManager.new()
	add_child(session_manager)

	# Wait for UI to be ready
	await get_tree().process_frame

	# Set up components
	file_manager.setup(file_list, code_edit)
	validator.setup(code_edit, file_manager)
	session_manager.setup(file_manager, resizable_container)

	# Connect toolbar signals
	menu_bar.new_file.connect(_on_new_button_pressed)
	menu_bar.open_file.connect(_on_open_button_pressed)
	menu_bar.save_requested.connect(_on_save_button_pressed)
	menu_bar.save_as_requested.connect(_on_save_as_button_pressed)
	menu_bar.undo_requested.connect(_on_undo_requested)
	menu_bar.redo_requested.connect(_on_redo_requested)

	# Connect code editor signals
	code_edit.content_changed.connect(_on_content_changed)
	code_edit.save_requested.connect(_on_save_button_pressed)
	code_edit.close_requested.connect(_on_close_current_file)
	code_edit.undo_requested.connect(_on_undo_requested)
	code_edit.redo_requested.connect(_on_redo_requested)
	code_edit.caret_changed.connect(_on_caret_changed)

	# Connect file manager signals
	file_manager.document_changed.connect(_on_document_changed)

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
	var document = file_manager.get_current_document()
	if not document:
		return

	if document.is_untitled():
		_on_save_as_button_pressed()
	else:
		file_manager.save_document(document)

func _on_save_as_button_pressed() -> void:
	var document = file_manager.get_current_document()
	if not document:
		return

	var file_dialog = EditorFileDialog.new()
	file_dialog.file_mode = EditorFileDialog.FILE_MODE_SAVE_FILE
	file_dialog.access = EditorFileDialog.ACCESS_FILESYSTEM
	file_dialog.add_filter("*.yaml;YAML Files")
	file_dialog.add_filter("*.yml;YML Files")
	file_dialog.title = "Save YAML File As"

	if not document.is_untitled():
		file_dialog.current_path = document.path

	file_dialog.file_selected.connect(
		func(path):
			file_manager.save_document_as(document, path)
			file_dialog.queue_free()
	)
	file_dialog.canceled.connect(func(): file_dialog.queue_free())

	add_child(file_dialog)
	file_dialog.popup_centered_ratio(0.7)

func _on_content_changed() -> void:
	var document = file_manager.get_current_document()
	if not document:
		return

	# Update document content
	file_manager.update_document_content(document, code_edit.text)

	# Request validation
	validator.validate_document(document)

func _on_close_current_file() -> void:
	var document = file_manager.get_current_document()
	if document:
		file_manager.close_document(document)

func _on_undo_requested() -> void:
	var document = file_manager.get_current_document()
	if not document:
		return

	var state = document.undo()
	if state:
		code_edit.set_text_and_preserve_state(document.content)

		# Optionally restore caret position if needed
		if state.caret_line > 0 and state.caret_column > 0:
			if state.caret_line < code_edit.get_line_count():
				code_edit.set_caret_line(state.caret_line)
				if state.caret_column <= code_edit.get_line(state.caret_line).length():
					code_edit.set_caret_column(state.caret_column)

func _on_redo_requested() -> void:
	var document = file_manager.get_current_document()
	if not document:
		return

	var state = document.redo()
	if state:
		code_edit.set_text_and_preserve_state(document.content)

		# Optionally restore caret position if needed
		if state.caret_line > 0 and state.caret_column > 0:
			if state.caret_line < code_edit.get_line_count():
				code_edit.set_caret_line(state.caret_line)
				if state.caret_column <= code_edit.get_line(state.caret_line).length():
					code_edit.set_caret_column(state.caret_column)

func _on_document_changed(document: YAMLDocument) -> void:
	# Update status panel with document info
	_update_line_col_label()

	# Show any validation errors
	if document.has_error():
		_display_validation_error(document)
	else:
		status_panel.set_status("")
		validator.clear_errors_in_editor()

func _on_caret_changed() -> void:
	status_panel.set_line_column(code_edit.get_current_line_col_info())

func _on_validation_completed(document: YAMLDocument) -> void:
	if document != file_manager.get_current_document():
		return

	if document.has_error():
		_display_validation_error(document)
	else:
		status_panel.set_status("")
		validator.clear_errors_in_editor()

func _display_validation_error(document: YAMLDocument) -> void:
	var result = document.validation_result
	if not result.has_error():
		return

	# Update status panel
	var error = result.get_error_message()
	var line = result.get_error_line()
	var col = result.get_error_column()

	var error_status := "Error at (%d, %d): %s" % [line, col, error] if line >= 0 else "Error: %s" % error
	status_panel.set_status(error_status, status_panel.ERROR_COLOR)

	# Mark error line in editor if possible
	if line >= 0:
		validator.mark_error_in_editor(line - 1, error)  # Convert to 0-based line

func _has_unsaved_changes() -> bool:
	return file_manager.has_unsaved_changes()

func get_open_files() -> Array:
	return file_manager.get_open_paths()

func handle_filesystem_change() -> void:
	file_manager.handle_filesystem_change()

func _notification(what):
	if what == NOTIFICATION_WM_CLOSE_REQUEST:
		# Save session when editor is closing
		session_manager.save_session()

func _update_line_col_label() -> void:
	# Allow one frame to pass to ensure the UI is updated
	await get_tree().process_frame
	_on_caret_changed()

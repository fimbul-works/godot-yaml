@tool
class_name YAMLEditor extends Control

# Components
var file_manager: YAMLEditorDocumentManager
var validator: YAMLEditorValidator
var session_manager: YAMLEditorSessionManager

# File system singleton
var file_system: YAMLFileSystem

# Editor reference
var editor: EditorInterface

# UI references
@export var menu_bar: YAMLEditorMenuBar
@export var file_list: YAMLEditorFileList
@export var resizable_container: HSplitContainer
@export var code_edit: YAMLCodeEditor
@export var status_panel: YAMLEditorStatusBar
@export var find_replace_panel: YAMLEditorFindReplaceBar

func _ready() -> void:
	# Get reference to file system singleton first
	file_system = YAMLFileSystem.get_singleton()

	# Initialize components
	file_manager = YAMLEditorDocumentManager.new()
	add_child(file_manager)

	validator = YAMLEditorValidator.new()
	add_child(validator)

	session_manager = YAMLEditorSessionManager.new()
	add_child(session_manager)

	# Wait for UI to be ready
	await get_tree().process_frame

	# Set up components
	file_manager.setup(file_list, code_edit)
	validator.setup(code_edit, file_manager)
	session_manager.setup(file_manager, resizable_container)

	# Connect menu signals for file operations
	menu_bar.new_file.connect(_on_new_button_pressed)
	menu_bar.open_file.connect(_on_open_button_pressed)
	menu_bar.save_requested.connect(_on_save_button_pressed)
	menu_bar.save_as_requested.connect(_on_save_as_button_pressed)
	menu_bar.close_requested.connect(_on_close_current_file)

	# Connect menu signals for edit options
	menu_bar.undo_requested.connect(_on_undo_requested)
	menu_bar.redo_requested.connect(_on_redo_requested)
	menu_bar.cut_requested.connect(_on_cut_requested)
	menu_bar.copy_requested.connect(_on_copy_requested)
	menu_bar.paste_requested.connect(_on_paste_requested)
	menu_bar.select_all_requested.connect(_on_select_all_requested)

	# Connect menu signals for search
	menu_bar.find_requested.connect(_on_find_requested)
	menu_bar.find_next_requested.connect(_on_find_next_requested)
	menu_bar.find_previous_requested.connect(_on_find_previous_requested)
	menu_bar.replace_requested.connect(_on_replace_requested)

	# Connect code editor signals
	code_edit.content_changed.connect(_on_content_changed)
	code_edit.save_requested.connect(_on_save_button_pressed)
	code_edit.close_requested.connect(_on_close_current_file)
	code_edit.undo_requested.connect(_on_undo_requested)
	code_edit.redo_requested.connect(_on_redo_requested)
	code_edit.caret_changed.connect(_on_caret_changed)
	code_edit.zoom_changed.connect(_on_zoom_changed)  # Connect to new zoom signal

	# Connect file manager signals
	file_manager.document_changed.connect(_on_document_changed)

	# Connect validation signals
	validator.validation_completed.connect(_on_validation_completed)

	# Set initial zoom text
	_on_zoom_changed(code_edit.zoom_level)

	# Share editor interface with components via tree metadata
	if editor:
		get_tree().set_meta("editor_interface", editor)

	# Setup the find and replace panels
	find_replace_panel.replace_performed.connect(_on_replace_performed)
	find_replace_panel.replace_all_performed.connect(_on_replace_all_performed)

	# Load previous session
	session_manager.load_session()

func _input(event):
	if event is InputEventKey and event.keycode == KEY_ESCAPE and event.pressed:
		if find_replace_panel.visible:
			find_replace_panel.hide_panel()
			get_viewport().set_input_as_handled()

func _unhandled_key_input(event: InputEvent) -> void:
	if event is InputEventKey and event.pressed:
		match event.get_keycode_with_modifiers():
			KEY_MASK_CTRL | KEY_F:
				_on_find_requested()
				get_viewport().set_input_as_handled()
			KEY_MASK_CTRL | KEY_R:
				_on_replace_requested()
				get_viewport().set_input_as_handled()
			KEY_F3:
				_on_find_next_requested()
				get_viewport().set_input_as_handled()
			KEY_MASK_SHIFT | KEY_F3:
				_on_find_previous_requested()
				get_viewport().set_input_as_handled()

func _on_zoom_changed(new_zoom_level: float) -> void:
	status_panel.set_zoom_level(new_zoom_level)

func _on_new_button_pressed() -> void:
	file_manager.new_file()

func _on_open_button_pressed() -> void:
	var file_dialog := EditorFileDialog.new()
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
	var document := file_manager.get_current_document()
	if not document:
		return

	if document.is_untitled():
		_on_save_as_button_pressed()
	else:
		file_manager.save_document(document)

func _on_save_as_button_pressed() -> void:
	var document := file_manager.get_current_document()
	if not document:
		return

	var file_dialog := EditorFileDialog.new()
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
	var document := file_manager.get_current_document()
	if not document:
		return

	# Update document content
	file_manager.update_document_content(document, code_edit.text)

	# Request validation
	validator.validate_document(document)

func _on_close_current_file() -> void:
	var document := file_manager.get_current_document()
	if document:
		file_manager.close_document(document)

func _on_undo_requested() -> void:
	var document := file_manager.get_current_document()
	if not document:
		return

	var state := document.undo()
	if not state:
		return

	code_edit.set_text_and_preserve_state(document.content)

	# Optionally restore caret position if needed
	if state.caret_line > 0 and state.caret_column > 0:
		if state.caret_line < code_edit.get_line_count():
			code_edit.set_caret_line(state.caret_line)
			if state.caret_column <= code_edit.get_line(state.caret_line).length():
				code_edit.set_caret_column(state.caret_column)

	# Validate after undo
	validator.validate_document(document)

	# And re-trigger search if we did undo
	if find_replace_panel.visible:
		find_replace_panel.trigger_search()

func _on_redo_requested() -> void:
	var document := file_manager.get_current_document()
	if not document:
		return

	var state := document.redo()
	if not state:
		return

	code_edit.set_text_and_preserve_state(document.content)

	# Optionally restore caret position if needed
	if state.caret_line > 0 and state.caret_column > 0:
		if state.caret_line < code_edit.get_line_count():
			code_edit.set_caret_line(state.caret_line)
			if state.caret_column <= code_edit.get_line(state.caret_line).length():
				code_edit.set_caret_column(state.caret_column)

	# Validate after redo
	validator.validate_document(document)

	# And re-trigger search if we did redo
	if find_replace_panel.visible:
		find_replace_panel.trigger_search()

func _on_cut_requested() -> void:
	code_edit.cut_selection()

func _on_copy_requested() -> void:
	code_edit.copy_selection()

func _on_paste_requested() -> void:
	code_edit.paste_clipboard()

func _on_select_all_requested() -> void:
	code_edit.select_all()

# Search-related methods
func _on_find_requested() -> void:
	find_replace_panel.show_find_panel()
	# Hide replace panel if we request just search
	if find_replace_panel.replace_panel_visible:
		find_replace_panel.replace_panel_visible = false

func _on_replace_requested() -> void:
	find_replace_panel.show_replace_panel()

func _on_find_next_requested() -> void:
	if find_replace_panel and find_replace_panel.visible:
		find_replace_panel.find_next()
	else:
		_on_find_requested()

func _on_find_previous_requested() -> void:
	if find_replace_panel and find_replace_panel.visible:
		find_replace_panel.find_previous()
	else:
		_on_find_requested()

func _on_replace_performed() -> void:
	# After a replace, request validation
	code_edit.validation_requested.emit()

	# Update the current document
	_on_content_changed()

func _on_replace_all_performed() -> void:
	# After replace all, request validation
	code_edit.validation_requested.emit()

	# Update the current document
	_on_content_changed()

	# Show a message in the status bar
	if find_replace_panel.visible and find_replace_panel.replace_panel.visible:
		status_panel.set_status("Replacement complete", Color.GREEN)
		# Clear the status after a delay
		if get_tree():
			await get_tree().create_timer(2.0).timeout
			status_panel.set_status("")

func _on_document_changed(document: YAMLEditorDocument) -> void:
	# Update status panel with document info
	_update_line_col_label()

	# Show any validation errors
	if document.has_error():
		_display_validation_error(document)
	else:
		status_panel.set_status("")
		validator.clear_errors_in_editor()

	# Re-trigger search
	if find_replace_panel.visible:
		find_replace_panel.trigger_search()

func _on_caret_changed() -> void:
	status_panel.set_line_column(code_edit.get_current_line_col_info())

func _on_validation_completed(document: YAMLEditorDocument) -> void:
	if document != file_manager.get_current_document():
		return

	if document.has_error():
		_display_validation_error(document)
	else:
		status_panel.set_status("")
		validator.clear_errors_in_editor()

func _display_validation_error(document: YAMLEditorDocument) -> void:
	status_panel.set_validation_result(document.validation_result)

	var result := document.validation_result
	if not result.has_error():
		return

	# Mark error line in editor if possible
	var error := result.get_error_message()
	var line := result.get_error_line()
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
	if get_tree():
		await get_tree().process_frame
		_on_caret_changed()

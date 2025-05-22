@tool
class_name YAMLEditorDocumentManager extends Node

signal document_changed(document)
signal document_created(document)
signal document_closed(document)

# Dictionary of open documents: {path: YAMLEditorDocument}
var documents: Dictionary = {}
var current_document: YAMLEditorDocument = null

# UI components
var file_list: YAMLEditorFileList
var code_editor: YAMLCodeEditor
var file_popup_menu: PopupMenu

# Reference to the singleton
var file_system: YAMLFileSystem

# Track recently saved files to avoid external update conflicts
var recently_saved_files: Dictionary = {}
var ignore_update_timer: Timer

func _ready() -> void:
	# Get singleton reference
	file_system = YAMLFileSystem.get_singleton()

	# Listen for external file updates
	file_system.file_updated.connect(_on_external_file_updated)
	file_system.file_renamed.connect(_on_file_renamed)

	# Create file popup menu
	file_popup_menu = PopupMenu.new()
	add_child(file_popup_menu)

	# Add menu items
	file_popup_menu.add_item("Save", 0)
	file_popup_menu.add_item("Save As...", 1)
	file_popup_menu.add_separator()
	file_popup_menu.add_item("Close", 2)
	file_popup_menu.add_separator()
	file_popup_menu.add_item("Show in FileSystem", 3)

	# Connect popup menu signals
	file_popup_menu.id_pressed.connect(_on_file_popup_menu_id_pressed)

	# Create timer for clearing recent saves
	ignore_update_timer = Timer.new()
	add_child(ignore_update_timer)
	ignore_update_timer.one_shot = true
	ignore_update_timer.wait_time = 0.5  # 500ms
	ignore_update_timer.timeout.connect(_on_ignore_update_timer_timeout)

func setup(p_file_list: YAMLEditorFileList, p_code_editor: YAMLCodeEditor) -> void:
	file_list = p_file_list
	code_editor = p_code_editor

	# Connect signals from file list component
	file_list.file_selected.connect(_on_file_selected)
	file_list.file_context_requested.connect(_on_file_context_requested)

func create_document(path: String, content: String = "") -> YAMLEditorDocument:
	var normalized_path = _normalize_path(path)

	# Check if document already exists with normalized path
	if documents.has(normalized_path):
		return documents[normalized_path]

	var document := YAMLEditorDocument.new(normalized_path, content)

	# Connect document signals
	document.content_changed.connect(_on_document_content_changed)
	document.modified_changed.connect(_on_document_modified_changed)

	# Store document
	documents[normalized_path] = document
	document_created.emit(document)

	return document

func open_file(path: String) -> void:
	var normalized_path = _normalize_path(path)

	# Check if already open
	if documents.has(normalized_path):
		set_current_document(documents[normalized_path])
		return

	# Look for any document with the same base filename
	var filename = normalized_path.get_file()
	for existing_path in documents.keys():
		if existing_path.get_file() == filename and existing_path != normalized_path:
			# Check if they point to the same actual file
			if _paths_point_to_same_file(normalized_path, existing_path):
				set_current_document(documents[existing_path])
				return

	# Use the file system singleton to read the file
	var content := file_system.read_file(normalized_path)
	if typeof(content) == TYPE_INT:  # Error code
		push_error("Could not open file '%s': %s" % [normalized_path, error_string(content)])
		return

	# Create new document
	var document := create_document(normalized_path, content)
	document.mark_saved()  # Initial state is saved

	# Switch to the new document
	set_current_document(document)

	# Notify the file system
	file_system.notify_file_opened(normalized_path)

func close_document(document: YAMLEditorDocument) -> bool:
	if document == null:
		return true

	if document.is_modified:
		# Show confirmation dialog for unsaved changes
		var dialog := ConfirmationDialog.new()
		dialog.title = "Unsaved Changes"
		dialog.dialog_text = "Save changes to '" + document.get_file_name() + "' before closing?"
		dialog.add_button("Don't Save", true, "dont_save")
		dialog.add_cancel_button("Cancel")

		dialog.confirmed.connect(
			func():
				# Save was chosen
				if save_document(document):
					_close_document_internal(document)
				dialog.queue_free()
		)

		dialog.custom_action.connect(
			func(action):
				if action == "dont_save":
					_close_document_internal(document)
				dialog.queue_free()
		)

		dialog.canceled.connect(func(): dialog.queue_free())

		add_child(dialog)
		dialog.popup_centered()
		return false

	return _close_document_internal(document)

func _close_document_internal(document: YAMLEditorDocument) -> bool:
	if document == null:
		return false

	# Find the document in our dictionary
	var path_to_remove = ""
	for path in documents.keys():
		if documents[path] == document:
			path_to_remove = path
			break

	if path_to_remove.is_empty():
		return false

	# Notify document is being closed
	document_closed.emit(document)

	# Remove document
	documents.erase(document.path)

	# If this was the current document, switch to another
	if current_document == document:
		current_document = null

		# Select another document if available
		if not documents.is_empty():
			set_current_document(documents.values()[0])
		else:
			# Clear the editor if no documents left
			code_editor.text = ""

	# Update UI
	update_ui()

	# Notify the file system
	file_system.notify_file_closed(document.path)

	return true

func save_document(document: YAMLEditorDocument) -> bool:
	if document == null:
		return false

	# Don't save untitled files directly
	if document.is_untitled():
		return false  # Caller should handle "Save As" dialog

	# Mark this file as recently saved to ignore update notifications
	recently_saved_files[document.path] = Time.get_unix_time_from_system()
	ignore_update_timer.start()

	# Use the file system singleton to save the file
	var result := file_system.save_file(document.path, document.content)
	if result != OK:
		push_error("Could not save file '%s': %s" % [document.path, error_string(result)])
		recently_saved_files.erase(document.path)  # Remove from recently saved if error
		return false

	# Mark document as saved
	document.mark_saved()

	# Update UI
	update_ui()

	return true

func save_document_as(document: YAMLEditorDocument, new_path: String) -> bool:
	if document == null or new_path.is_empty():
		return false

	var normalized_new_path = _normalize_path(new_path)

	# Check if we're trying to save to a path that's already open
	if documents.has(normalized_new_path) and documents[normalized_new_path] != document:
		push_error("Cannot save as '%s' - file is already open" % normalized_new_path)
		return false

	# Remember the old path
	var old_path := document.path

	# Update document path
	document.path = normalized_new_path

	# Update the documents dictionary
	if old_path != normalized_new_path:
		documents.erase(old_path)
		documents[normalized_new_path] = document

	# Save the document
	if save_document(document):
		# If old path was temporary, clean up
		if old_path != normalized_new_path  and old_path.begins_with("untitled"):
			file_system.notify_file_closed(old_path)

		# Update UI
		update_ui()

		# Notify file system
		file_system.notify_file_opened(new_path)

		return true

	# Restore old path if save failed
	if old_path != normalized_new_path:
		document.path = old_path
		documents.erase(normalized_new_path)
		documents[old_path] = document

	return false

func new_file() -> void:
	# Create a new untitled file
	var untitled_name := "untitled.yaml"
	var index := 1

	while documents.has(untitled_name):
		index += 1
		untitled_name = "untitled%d.yaml" % index

	# Create a new document
	var document := create_document(untitled_name)
	document.set_modified(true)  # New document is always modified

	# Switch to the new document
	set_current_document(document)

	# Set focus to code editor
	code_editor.grab_focus()

	# Notify the file system
	file_system.notify_file_opened(untitled_name)

func set_current_document(document: YAMLEditorDocument) -> void:
	if document == null or document == current_document:
		return

	current_document = document

	# Update editor content
	if is_instance_valid(code_editor):
		code_editor.set_text_and_preserve_state(document.content)

	# Update UI
	update_ui()

	# Emit signal
	document_changed.emit(document)

func update_document_content(document: YAMLEditorDocument, new_content: String) -> void:
	if document == null:
		return

	var caret_line := 0
	var caret_column := 0

	if is_instance_valid(code_editor):
		caret_line = code_editor.get_caret_line()
		caret_column = code_editor.get_caret_column()

	document.set_content(new_content, caret_line, caret_column)

func _normalize_path(path: String) -> String:
	# Convert to absolute path and normalize
	var normalized = path

	# Handle different path formats
	if normalized.begins_with("res://"):
		normalized = ProjectSettings.globalize_path(normalized)

	# Convert to canonical form
	normalized = normalized.simplify_path()

	# Convert back to res:// format if it was originally a project path
	if path.begins_with("res://"):
		normalized = ProjectSettings.localize_path(normalized)

	return normalized

func _paths_point_to_same_file(path1: String, path2: String) -> bool:
	# For project files, compare the res:// paths
	if path1.begins_with("res://") and path2.begins_with("res://"):
		return path1 == path2

	# For absolute paths, normalize and compare
	var abs_path1 = ProjectSettings.globalize_path(path1) if path1.begins_with("res://") else path1
	var abs_path2 = ProjectSettings.globalize_path(path2) if path2.begins_with("res://") else path2

	return abs_path1.simplify_path() == abs_path2.simplify_path()

func _on_document_content_changed(document: YAMLEditorDocument) -> void:
	# Update UI if this is the current document
	if document == current_document:
		update_ui()

func _on_document_modified_changed(document: YAMLEditorDocument) -> void:
	# Update UI if this is the current document
	if document == current_document:
		update_ui()

func update_ui() -> void:
	if not is_instance_valid(file_list):
		return

	# Prepare file data for the file list component
	var file_data := {}
	for path in documents:
		var document = documents[path]
		file_data[path] = {
			"name": document.get_file_name(),
			"modified": document.is_modified
		}

	# Update the file list component
	var current_path = current_document.path if current_document else ""
	file_list.update_files(file_data, current_path)

func _on_file_selected(path: String) -> void:
	if path.is_empty():
		return

	var normalized_path = _normalize_path(path)
	if not documents.has(normalized_path):
		return

	set_current_document(documents[normalized_path])

func _on_file_context_requested(path: String, at_position: Vector2) -> void:
	if path.is_empty():
		return

	var normalized_path = _normalize_path(path)
	if not documents.has(normalized_path):
		return

	set_current_document(documents[normalized_path])

	# Calculate the global position for the popup
	var global_rect := Rect2(file_list.get_global_mouse_position(), Vector2.ZERO)
	file_popup_menu.popup_on_parent(global_rect)

func _on_file_popup_menu_id_pressed(id: int) -> void:
	var path := file_list.get_selected_file_path()
	if path.is_empty():
		return

	var normalized_path = _normalize_path(path)
	if not documents.has(normalized_path):
		return

	var document: YAMLEditorDocument = documents[normalized_path]

	match id:
		0:  # Save
			save_document(document)
		1:  # Save As
			# Main editor should handle the save as dialog
			set_current_document(document)
		2:  # Close
			close_document(document)
		3:  # Show in FileSystem
			if not document.is_untitled() and document.path.begins_with("res://"):
				EditorInterface.get_file_system_dock().navigate_to_path(document.path)

func _on_external_file_updated(path: String) -> void:
	var normalized_path = _normalize_path(path)

	# Only process if the file is open and it's a YAML file
	if documents.has(normalized_path) and file_system.is_yaml_file(normalized_path):
		# Check if we just saved this file ourselves
		if recently_saved_files.has(normalized_path):
			var save_time: int = recently_saved_files[normalized_path]
			var current_time := Time.get_unix_time_from_system()

			# If saved less than 1 second ago, ignore this update
			if current_time - save_time < 1.0:
				return

		var document: YAMLEditorDocument = documents[normalized_path]

		# Check if the document has unsaved changes
		if not document.is_modified:
			# Document is not modified locally, safe to reload
			var content = file_system.read_file(normalized_path)
			if typeof(content) != TYPE_INT:  # Not an error
				# Update document content
				document.content = content
				document.mark_saved()

				# If this is the current document, update the editor
				if document == current_document:
					# Preserve cursor position and scroll state
					var previous_caret_line := code_editor.get_caret_line()
					var previous_caret_column := code_editor.get_caret_column()
					var previous_scroll_v := code_editor.get_v_scroll_bar().value
					var previous_scroll_h := code_editor.get_h_scroll_bar().value

					code_editor.text = content

					# Restore position if possible
					if previous_caret_line < code_editor.get_line_count():
						code_editor.set_caret_line(previous_caret_line)
						var line_length := code_editor.get_line(previous_caret_line).length()
						if previous_caret_column <= line_length:
							code_editor.set_caret_column(previous_caret_column)

					# Restore scroll position
					code_editor.get_v_scroll_bar().value = previous_scroll_v
					code_editor.get_h_scroll_bar().value = previous_scroll_h

					update_ui()
		else:
			# Document has unsaved changes, show conflict dialog
			if document == current_document:
				var dialog := ConfirmationDialog.new()
				dialog.title = "External Changes Detected"
				dialog.dialog_text = "The file '" + document.get_file_name() + "' has been modified externally. Do you want to reload it and lose your changes?"
				dialog.confirmed.connect(
					func():
						var content := file_system.read_file(path)
						if typeof(content) != TYPE_INT:
							document.content = content
							document.mark_saved()

							if document == current_document:
								code_editor.text = content

							update_ui()
						dialog.queue_free()
				)
				dialog.canceled.connect(func(): dialog.queue_free())
				add_child(dialog)
				dialog.popup_centered()

func _on_ignore_update_timer_timeout() -> void:
	# Clear out any old saved entries
	var current_time := Time.get_unix_time_from_system()
	var keys_to_remove: PackedStringArray = []

	for path in recently_saved_files:
		var save_time = recently_saved_files[path]
		if current_time - save_time >= 1.0:
			keys_to_remove.append(path)

	for path in keys_to_remove:
		recently_saved_files.erase(path)

func _on_file_renamed(old_path: String, new_path: String) -> void:
	var normalized_old_path = _normalize_path(old_path)
	var normalized_new_path = _normalize_path(new_path)

	# If we have this document open, update our references
	if documents.has(normalized_old_path):
		var document: YAMLEditorDocument = documents[normalized_old_path]
		document.path = normalized_new_path

		documents.erase(normalized_old_path)
		documents[normalized_new_path] = document

		update_ui()

func handle_filesystem_change() -> void:
	# Check if any of our open res:// files no longer exist
	var missing_files: PackedStringArray = []

	for path in documents.keys():
		if path.begins_with("res://") and not file_system.file_exists(path):
			missing_files.append(path)

	# Handle missing files
	for old_path in missing_files:
		var document: YAMLEditorDocument = documents[old_path]

		# Try to find a file with the same name but different path in the filesystem
		var filename := old_path.get_file()
		var filesystem_root := EditorInterface.get_resource_filesystem().get_filesystem()
		var new_path := file_system.find_file_in_filesystem(filesystem_root, filename)

		if not new_path.is_empty():
			var normalized_new_path = _normalize_path(new_path)

			# Found potential match - update the document path
			document.path = normalized_new_path
			documents.erase(old_path)
			documents[normalized_new_path] = document

			# If this is the current document, emit signal
			if document == current_document:
				document_changed.emit(document)

			# Update UI
			update_ui()

			# Notify file system
			file_system.notify_file_closed(old_path)
			file_system.notify_file_opened(normalized_new_path)
			file_system.notify_file_renamed(old_path, normalized_new_path)
		else:
			# Keep it open but mark as potentially moved/deleted to avoid losing unsaved changes
			pass

func has_unsaved_changes() -> bool:
	for document in documents.values():
		if document.is_modified:
			return true
	return false

func get_open_documents() -> Array:
	return documents.values()

func get_open_paths() -> Array:
	return documents.keys()

func has_document(path: String) -> bool:
	return documents.has(path)

func get_document(path: String) -> YAMLEditorDocument:
	return documents.get(path, null)

func get_current_document() -> YAMLEditorDocument:
	return current_document

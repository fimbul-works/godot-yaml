@tool
class_name YAMLFileManager extends Node

signal current_file_changed(path)
signal file_modified(path, is_modified)

# Dictionary of open files: {path: {content, modified, saved_state}}
var open_files: Dictionary = {}
var current_file_path: String = ""
var unsaved_changes: bool = false

# UI components
var file_list: YAMLFileList
var code_editor: CodeEdit
var file_popup_menu: PopupMenu

# Reference to the singleton
var file_system: YAMLFileSystem

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

func setup(p_file_list: YAMLFileList, p_code_editor: CodeEdit) -> void:
	file_list = p_file_list
	code_editor = p_code_editor

	# Connect signals from file list component
	file_list.file_selected.connect(_on_file_selected)
	file_list.file_context_requested.connect(_on_file_context_requested)

func open_file(path: String) -> void:
	# Check if already open
	if open_files.has(path):
		current_file_path = path
		update_ui()
		load_current_file_content()
		return

	# Use the file system singleton to read the file
	var content = file_system.read_file(path)
	if typeof(content) == TYPE_INT:  # Error code
		push_error("Could not open file: ", path)
		return

	# Add to open files
	open_files[path] = {
		"modified": false,
		"content": content,
		"saved_state": content  # Store the saved state
	}

	current_file_path = path
	update_ui()
	load_current_file_content()

	# Notify the file system
	file_system.notify_file_opened(path)

func close_file(path: String) -> bool:
	if not open_files.has(path):
		return true

	if open_files[path].modified:
		# Show confirmation dialog for unsaved changes
		var dialog = ConfirmationDialog.new()
		dialog.title = "Unsaved Changes"
		dialog.dialog_text = "Save changes to '" + path.get_file() + "' before closing?"
		dialog.add_button("Don't Save", true, "dont_save")
		dialog.add_cancel_button("Cancel")

		dialog.confirmed.connect(
			func():
				# Save was chosen
				if path == current_file_path:
					if save_current_file():
						_close_file_internal(path)
				else:
					var old_path = current_file_path
					current_file_path = path
					if save_current_file():
						_close_file_internal(path)
					current_file_path = old_path
				dialog.queue_free()
		)

		dialog.custom_action.connect(
			func(action):
				if action == "dont_save":
					_close_file_internal(path)
				dialog.queue_free()
		)

		dialog.canceled.connect(func(): dialog.queue_free())

		add_child(dialog)
		dialog.popup_centered()
		return false

	return _close_file_internal(path)

func _close_file_internal(path: String) -> bool:
	if not open_files.has(path):
		return false

	open_files.erase(path)

	if current_file_path == path:
		current_file_path = ""
		code_editor.text = ""

		# Select another file if available
		if not open_files.is_empty():
			current_file_path = open_files.keys()[0]
			load_current_file_content()

	update_ui()

	# Notify the file system
	file_system.notify_file_closed(path)

	# Update current file if needed
	if current_file_path != path and !current_file_path.is_empty():
		current_file_changed.emit(current_file_path)

	return true

func save_current_file() -> bool:
	if current_file_path.is_empty():
		return false

	# Don't save untitled files directly
	if current_file_path.begins_with("untitled"):
		return false  # Caller should handle "Save As" dialog

	# Use the file system singleton to save the file
	var result = file_system.save_file(current_file_path, code_editor.text)
	if result != OK:
		push_error("Could not save file: ", current_file_path)
		return false

	# Update state
	open_files[current_file_path].content = code_editor.text
	open_files[current_file_path].modified = false
	open_files[current_file_path].saved_state = code_editor.text  # Update saved state
	unsaved_changes = false

	update_ui()

	return true

func save_file_as(path: String) -> bool:
	if path.is_empty():
		return false

	var old_path = current_file_path
	current_file_path = path

	if save_current_file():
		# If old path was temporary, remove it
		if not old_path.is_empty() and old_path != path and old_path.begins_with("untitled"):
			open_files.erase(old_path)
			file_system.notify_file_closed(old_path)

		update_ui()

		# If this is a new path, notify opened
		if old_path != path:
			file_system.notify_file_opened(path)
			current_file_changed.emit(path)

		return true

	current_file_path = old_path
	return false

func new_file() -> void:
	# Create a new untitled file
	var untitled_name = "untitled.yaml"
	var index = 1

	while open_files.has(untitled_name):
		index += 1
		untitled_name = "untitled%d.yaml" % index

	open_files[untitled_name] = {
		"modified": true,
		"content": "",
		"saved_state": ""  # Empty saved state for new files
	}

	current_file_path = untitled_name
	code_editor.text = ""
	unsaved_changes = true
	update_ui()

	# Set focus to code editor
	code_editor.grab_focus()

	# Emit signals
	file_system.notify_file_opened(untitled_name)
	current_file_changed.emit(untitled_name)
	file_modified.emit(untitled_name, true)

func set_current_file_content(content: String) -> void:
	if current_file_path.is_empty() or not open_files.has(current_file_path):
		return

	code_editor.text = content
	open_files[current_file_path].content = content

func mark_current_modified(is_modified: bool) -> void:
	if current_file_path.is_empty() or not open_files.has(current_file_path):
		return

	if open_files[current_file_path].modified != is_modified:
		open_files[current_file_path].modified = is_modified
		unsaved_changes = is_modified
		update_ui()
		file_modified.emit(current_file_path, is_modified)

func get_current_file_content() -> String:
	if current_file_path.is_empty() or not open_files.has(current_file_path):
		return ""

	return code_editor.text

func load_current_file_content() -> void:
	if current_file_path.is_empty() or not open_files.has(current_file_path):
		code_editor.text = ""
		return

	code_editor.text = open_files[current_file_path].content
	unsaved_changes = open_files[current_file_path].modified
	update_ui()

	# Update content in case it changed externally
	open_files[current_file_path].content = code_editor.text

func handle_filesystem_change() -> void:
	# Handle file renaming/moving

	# Check if any of our open res:// files no longer exist
	var missing_files = []

	for old_path in open_files.keys():
		if old_path.begins_with("res://") and not file_system.file_exists(old_path):
			missing_files.append(old_path)

	# Handle missing files
	for old_path in missing_files:
		# Try to find a file with the same name but different path in the filesystem
		var filename = old_path.get_file()
		var filesystem_root = EditorInterface.get_resource_filesystem().get_filesystem()
		var new_path = file_system.find_file_in_filesystem(filesystem_root, filename)

		if not new_path.is_empty():
			# Found potential match - update the file path
			_update_file_path(old_path, new_path)
		else:
			# Keep it open but mark as potentially moved/deleted to avoid losing unsaved changes
			pass

func _update_file_path(old_path: String, new_path: String) -> void:
	if not open_files.has(old_path):
		return

	# Copy the file data to the new path
	var file_data = open_files[old_path]
	open_files[new_path] = file_data
	open_files.erase(old_path)

	# Update current file path if needed
	if current_file_path == old_path:
		current_file_path = new_path
		current_file_changed.emit(new_path)

	# Update UI
	update_ui()

	# Notify the file system
	file_system.notify_file_closed(old_path)
	file_system.notify_file_opened(new_path)
	file_system.notify_file_renamed(old_path, new_path)

func _on_external_file_updated(path: String) -> void:
	# Only process if the file is open and it's a YAML file
	if open_files.has(path) and file_system.is_yaml_file(path):
		# Check if the file has unsaved changes
		if not open_files[path].modified:
			# File is not modified locally, safe to reload
			var content = file_system.read_file(path)
			if typeof(content) != TYPE_INT:  # Not an error
				open_files[path].content = content
				open_files[path].saved_state = content

				# If this is the current file, update the editor
				if path == current_file_path:
					# Preserve cursor position and scroll state
					var previous_caret_line = code_editor.get_caret_line()
					var previous_caret_column = code_editor.get_caret_column()
					var previous_scroll_v = code_editor.get_v_scroll_bar().value
					var previous_scroll_h = code_editor.get_h_scroll_bar().value

					code_editor.text = content

					# Restore position if possible
					if previous_caret_line < code_editor.get_line_count():
						code_editor.set_caret_line(previous_caret_line)
						var line_length = code_editor.get_line(previous_caret_line).length()
						if previous_caret_column <= line_length:
							code_editor.set_caret_column(previous_caret_column)

					# Restore scroll position
					code_editor.get_v_scroll_bar().value = previous_scroll_v
					code_editor.get_h_scroll_bar().value = previous_scroll_h

					unsaved_changes = false
					update_ui()
		else:
			# File has unsaved changes, show conflict dialog
			if path == current_file_path:
				var dialog = ConfirmationDialog.new()
				dialog.title = "External Changes Detected"
				dialog.dialog_text = "The file '" + path.get_file() + "' has been modified externally. Do you want to reload it and lose your changes?"
				dialog.confirmed.connect(
					func():
						var content = file_system.read_file(path)
						if typeof(content) != TYPE_INT:
							code_editor.text = content
							open_files[path].content = content
							open_files[path].modified = false
							open_files[path].saved_state = content
							unsaved_changes = false
							update_ui()
						dialog.queue_free()
				)
				dialog.canceled.connect(func(): dialog.queue_free())
				add_child(dialog)
				dialog.popup_centered()

func _on_file_renamed(old_path: String, new_path: String) -> void:
	# If we have this file open, update our references
	if open_files.has(old_path):
		_update_file_path(old_path, new_path)

func update_ui() -> void:
	if not is_instance_valid(file_list):
		return

	# Prepare file data for the file list component
	var file_data = {}
	for path in open_files:
		file_data[path] = {
			"name": path.get_file(),
			"modified": open_files[path].modified
		}

	# Update the file list component
	file_list.update_files(file_data, current_file_path)

func _on_file_context_requested(path: String, at_position: Vector2) -> void:
	if path.is_empty():
		return

	if path != current_file_path:
		current_file_path = path
		load_current_file_content()
		current_file_changed.emit(path)

	# Calculate the global position for the popup
	var global_rect = Rect2(file_list.get_global_mouse_position(), Vector2.ZERO)
	file_popup_menu.popup_on_parent(global_rect)

func _on_file_popup_menu_id_pressed(id: int) -> void:
	var path = file_list.get_selected_file_path()
	if path.is_empty():
		return

	match id:
		0:  # Save
			if path == current_file_path:
				save_current_file()
			else:
				# Switch to file and save
				current_file_path = path
				load_current_file_content()
				current_file_changed.emit(path)
				save_current_file()
		1:  # Save As
			if path != current_file_path:
				current_file_path = path
				load_current_file_content()
				current_file_changed.emit(path)
			# Main editor should handle the save as dialog
		2:  # Close
			close_file(path)
		3:  # Show in FileSystem
			if not path.begins_with("untitled") and path.begins_with("res://"):
				EditorInterface.get_file_system_dock().navigate_to_path(path)

func _on_file_selected(path: String) -> void:
	if path.is_empty() or path == current_file_path:
		return

	current_file_path = path
	load_current_file_content()
	current_file_changed.emit(path)

func has_unsaved_changes() -> bool:
	return unsaved_changes

func get_open_files() -> Array:
	return open_files.keys()

func has_file_open(path: String) -> bool:
	return open_files.has(path)

func get_current_file_path() -> String:
	return current_file_path

@tool
extends Control

var editor: EditorInterface
var syntax_highlighter: EditorSyntaxHighlighter = preload("res://addons/yaml/editor/yaml_syntax_highlighter.gd").new()
var current_file_path: String = ""
var open_files: Dictionary = {}  # path: {modified, content}
var unsaved_changes: bool = false
var validation_timer: Timer
var file_popup_menu: PopupMenu

@onready var file_list = %FileList
@onready var code_edit = %YAMLCodeEdit
@onready var status_label = %StatusLabel
@onready var line_col_label = %LineColLabel

func _ready() -> void:
	# Set up syntax highlighter
	code_edit.syntax_highlighter = syntax_highlighter

	# Set up validation timer
	validation_timer = Timer.new()
	validation_timer.one_shot = true
	validation_timer.wait_time = 0.5  # 500ms delay
	validation_timer.timeout.connect(_validate_yaml)
	add_child(validation_timer)

	# Connect code edit signals for position tracking
	code_edit.caret_changed.connect(_update_line_col_info)

	# Setup file popup menu
	_setup_file_popup_menu()

	# Connect file list signals
	file_list.item_clicked.connect(_on_file_item_clicked)
	file_list.item_selected.connect(_on_file_selected)

	# Clear any example items
	file_list.clear()

	# Update UI state
	_update_ui()

func _setup_file_popup_menu() -> void:
	file_popup_menu = PopupMenu.new()
	add_child(file_popup_menu)

	# Add menu items similar to Godot's script editor
	file_popup_menu.add_item("Save", 0)
	file_popup_menu.add_item("Save As...", 1)
	file_popup_menu.add_separator()
	file_popup_menu.add_item("Close", 2)
	file_popup_menu.add_separator()
	file_popup_menu.add_item("Show in FileSystem", 3)

	# Connect signals
	file_popup_menu.id_pressed.connect(_on_file_popup_menu_id_pressed)

func _on_file_item_clicked(index: int, at_position: Vector2, mouse_button_index: int) -> void:
	if mouse_button_index == MOUSE_BUTTON_RIGHT:
		# Select the clicked item
		file_list.select(index)
		current_file_path = file_list.get_item_metadata(index)
		_load_current_file_content()

		# Show context menu
		file_popup_menu.position = file_list.global_position + at_position
		file_popup_menu.popup()

func _on_file_popup_menu_id_pressed(id: int) -> void:
	var selected_idx = file_list.get_selected_items()
	if selected_idx.is_empty():
		return

	var path = file_list.get_item_metadata(selected_idx[0])

	match id:
		0:  # Save
			if path == current_file_path:
				save_current_file()
			else:
				# Switch to file and save
				current_file_path = path
				_load_current_file_content()
				save_current_file()
		1:  # Save As
			if path != current_file_path:
				current_file_path = path
				_load_current_file_content()
			_on_save_as_button_pressed()
		2:  # Close
			close_file(path)
		3:  # Show in FileSystem
			if not path.begins_with("untitled") and path.begins_with("res://"):
				if editor:  # Only works if editor reference is set
					editor.get_file_system_dock().navigate_to_path(path)

func _update_ui() -> void:
	# Update file list
	file_list.clear()

	var current_index := -1
	var index := 0

	for path in open_files:
		var file_name = path.get_file()
		var modified = open_files[path].modified

		if modified:
			file_name += " (*)"

		file_list.add_item(file_name)
		file_list.set_item_metadata(index, path)

		if path == current_file_path:
			current_index = index

		index += 1

	if current_index >= 0:
		file_list.select(current_index)

	# Update status
	if current_file_path.is_empty():
		status_label.text = "No file open"
	else:
		var modified = ""
		if unsaved_changes:
			modified = " (*)"
		status_label.text = current_file_path.get_file() + modified

func _update_line_col_info() -> void:
	var line = code_edit.get_caret_line() + 1
	var col = code_edit.get_caret_column() + 1
	line_col_label.text = "%d : %d" % [line, col]

func _validate_yaml() -> void:
	for i in range(code_edit.get_line_count()):
		code_edit.set_line_background_color(i, Color(0, 0, 0, 0))
		code_edit.set_line_gutter_icon(i, 0, null)

	if code_edit.text.strip_edges().is_empty():
		return

	# Validate the YAML content
	var result = YAML.validate(code_edit.text)

	if result.has_error():

		var line = result.get_error_line()
		var col = result.get_error_column()
		status_label.modulate = Color.html("#ff6f6f")
		status_label.text = "Error at (%d, %d): %s" % [
			line,
			col,
			result.get_error_message()
		]

		var error_color: Color = EditorInterface.get_editor_settings().get_setting("text_editor/theme/highlighting/brace_mismatch_color")

		var error_mark_color: Color = EditorInterface.get_editor_settings().get_setting("text_editor/theme/highlighting/mark_color")
		if line >= 0 and line < code_edit.get_line_count():
			code_edit.set_line_background_color(line, error_mark_color)

			if theme and theme.has_icon("StatusError", "EditorIcons"):
				code_edit.set_line_gutter_icon(line, 0, theme.get_icon("StatusError", "EditorIcons"))
	else:
		status_label.modulate = Color.WHITE
		status_label.text = "YAML valid"

func open_file(path: String) -> void:
	# Check if already open
	if open_files.has(path):
		current_file_path = path
		_update_ui()
		_load_current_file_content()
		return

	# Open the file
	var file = FileAccess.open(path, FileAccess.READ)
	if not file:
		printerr("Could not open file: ", path)
		return

	var content = file.get_as_text()

	# Add to open files
	open_files[path] = {
		"modified": false,
		"content": content
	}

	current_file_path = path
	_update_ui()
	_load_current_file_content()

func _load_current_file_content() -> void:
	if current_file_path.is_empty() or not open_files.has(current_file_path):
		code_edit.text = ""
		return

	code_edit.text = open_files[current_file_path].content
	unsaved_changes = open_files[current_file_path].modified
	_update_ui()

	# Validate after loading
	validation_timer.start()

func save_current_file() -> bool:
	if current_file_path.is_empty():
		return false

	# Don't save untitled files directly
	if current_file_path.begins_with("untitled"):
		_on_save_as_button_pressed()
		return false

	var file = FileAccess.open(current_file_path, FileAccess.WRITE)
	if not file:
		printerr("Could not save file: ", current_file_path)
		return false

	file.store_string(code_edit.text)

	# Update state
	open_files[current_file_path].content = code_edit.text
	open_files[current_file_path].modified = false
	unsaved_changes = false
	_update_ui()

	status_label.text = "Saved: " + current_file_path.get_file()
	return true

func save_file_as(path: String) -> bool:
	var old_path = current_file_path
	current_file_path = path

	if save_current_file():
		# If old path was temporary or shouldn't be kept, remove it
		if not old_path.is_empty() and old_path != path and old_path.begins_with("untitled"):
			open_files.erase(old_path)

		_update_ui()
		return true

	current_file_path = old_path
	return false

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
	open_files.erase(path)

	if current_file_path == path:
		current_file_path = ""
		code_edit.text = ""

		# Select another file if available
		if not open_files.is_empty():
			current_file_path = open_files.keys()[0]
			_load_current_file_content()

	_update_ui()
	return true

func new_file() -> void:
	# Create a new untitled file
	var untitled_name = "untitled.yaml"
	var index = 1

	while open_files.has(untitled_name):
		index += 1
		untitled_name = "untitled%d.yaml" % index

	open_files[untitled_name] = {
		"modified": true,
		"content": ""
	}

	current_file_path = untitled_name
	code_edit.text = ""
	unsaved_changes = true
	_update_ui()

# Signal handlers
func _on_new_button_pressed() -> void:
	new_file()

func _on_open_button_pressed() -> void:
	var file_dialog = EditorFileDialog.new()
	file_dialog.file_mode = EditorFileDialog.FILE_MODE_OPEN_FILE
	file_dialog.access = EditorFileDialog.ACCESS_FILESYSTEM
	file_dialog.add_filter("*.yaml;YAML Files")
	file_dialog.add_filter("*.yml;YML Files")
	file_dialog.title = "Open YAML File"

	# Connect signals
	file_dialog.file_selected.connect(
		func(path):
			open_file(path)
			file_dialog.queue_free()
	)
	file_dialog.canceled.connect(func(): file_dialog.queue_free())

	# Add dialog to the tree and show it
	add_child(file_dialog)
	file_dialog.popup_centered_ratio(0.7)

func _on_file_selected(index: int) -> void:
	if index < 0 or index >= file_list.get_item_count():
		return

	var path = file_list.get_item_metadata(index)
	if path != current_file_path:
		current_file_path = path
		_load_current_file_content()

func _on_save_button_pressed() -> void:
	if current_file_path.is_empty() or current_file_path.begins_with("untitled"):
		_on_save_as_button_pressed()
	else:
		save_current_file()

func _on_save_as_button_pressed() -> void:
	var file_dialog = EditorFileDialog.new()
	file_dialog.file_mode = EditorFileDialog.FILE_MODE_SAVE_FILE
	file_dialog.access = EditorFileDialog.ACCESS_FILESYSTEM
	file_dialog.add_filter("*.yaml;YAML Files")
	file_dialog.add_filter("*.yml;YML Files")
	file_dialog.title = "Save YAML File As"

	# Connect signals
	file_dialog.file_selected.connect(
		func(path):
			save_file_as(path)
			file_dialog.queue_free()
	)
	file_dialog.canceled.connect(func(): file_dialog.queue_free())

	# Add dialog to the tree and show it
	add_child(file_dialog)
	file_dialog.popup_centered_ratio(0.7)

func _on_validation_button_pressed() -> void:
	_validate_yaml()

func _on_code_edit_text_changed() -> void:
	if not current_file_path.is_empty():
		unsaved_changes = true
		open_files[current_file_path].modified = true
		_update_ui()

	# Start validation timer
	validation_timer.stop()
	validation_timer.start()

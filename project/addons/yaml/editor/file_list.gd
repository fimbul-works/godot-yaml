@tool
class_name YAMLFileList extends ItemList

signal file_selected(path)
signal file_context_requested(path, position)

# References to other components
@export var filter_input: LineEdit

# File data
var files: Dictionary = {}  # {path: {name, modified}}
var filtered_files: Array = []
var current_path: String = ""

func _ready() -> void:
	# Connect internal signals
	item_selected.connect(_on_item_selected)
	item_clicked.connect(_on_item_clicked)

	# Connect filter input signal if available
	if is_instance_valid(filter_input):
		filter_input.text_changed.connect(_on_filter_text_changed)

# API for other components to update the file list
func update_files(p_files: Dictionary, p_current_path: String) -> void:
	files = p_files.duplicate()
	current_path = p_current_path
	_update_ui()

func mark_file_modified(path: String, is_modified: bool) -> void:
	if files.has(path):
		files[path].modified = is_modified
		_update_ui()

func get_selected_file_path() -> String:
	var selected_items = get_selected_items()
	if selected_items.is_empty():
		return ""

	var selected_index = selected_items[0]
	if selected_index >= 0 and selected_index < filtered_files.size():
		return filtered_files[selected_index]

	return ""

# UI Update functions
func _update_ui() -> void:
	# Store current selection
	var current_selection = get_selected_file_path()

	# Clear and repopulate
	clear()
	filtered_files.clear()

	# Apply filter
	var filter_text = filter_input.text.to_lower() if is_instance_valid(filter_input) else ""

	var current_index = -1
	var index = 0

	# Add matching files to the list
	for path in files.keys():
		var file_data = files[path]
		var file_name = path.get_file()

		# Apply filter if needed
		if not filter_text.is_empty() and file_name.to_lower().find(filter_text) == -1:
			continue

		# Format display name
		var display_name = file_name
		if file_data.modified:
			display_name += " (*)"

		# Add to list and store in filtered array
		add_item(display_name)
		filtered_files.append(path)

		# Check if this is the current file
		if path == current_path:
			current_index = index

		# Check if this was previously selected
		if path == current_selection:
			select(index)

		index += 1

	# Select current file if no previous selection
	if current_index >= 0 and get_selected_items().is_empty():
		select(current_index)

# Signal handlers
func _on_filter_text_changed(_text: String) -> void:
	_update_ui()

func _on_item_selected(index: int) -> void:
	if index >= 0 and index < filtered_files.size():
		file_selected.emit(filtered_files[index])

func _on_item_clicked(index: int, at_position: Vector2, mouse_button_index: int) -> void:
	if index >= 0 and index < filtered_files.size():
		if mouse_button_index == MOUSE_BUTTON_RIGHT:
			# Request context menu
			select(index)
			file_context_requested.emit(filtered_files[index], at_position)

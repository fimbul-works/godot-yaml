@tool
class_name YAMLEditorFileList extends VBoxContainer

signal file_selected(path)
signal file_context_requested(path, position)

# References to UI components
@export var filter_input: LineEdit
@export var file_list: ItemList

# File data
var files: Dictionary = {}  # {path: {name, modified}}
var filtered_files: Array = []
var current_path: String = ""

func _ready() -> void:
	# SessionManager will handle loading of the files
	file_list.clear()

	# Connect internal signals
	if is_instance_valid(file_list):
		file_list.item_selected.connect(_on_item_selected)
		file_list.item_clicked.connect(_on_item_clicked)

	if is_instance_valid(filter_input):
		filter_input.text_changed.connect(_on_filter_text_changed)
		filter_input.right_icon = get_theme_icon("Search", "EditorIcons")

# Public API
func update_files(p_files: Dictionary, p_current_path: String) -> void:
	files = p_files.duplicate()
	current_path = p_current_path
	_update_ui()

func mark_file_modified(path: String, is_modified: bool) -> void:
	if files.has(path):
		files[path].modified = is_modified
		_update_ui()

func get_selected_file_path() -> String:
	if not is_instance_valid(file_list):
		return ""

	var selected_items := file_list.get_selected_items()
	if selected_items.is_empty():
		return ""

	var selected_index := selected_items[0]
	if selected_index >= 0 and selected_index < filtered_files.size():
		return filtered_files[selected_index]

	return ""

# UI update
func _update_ui() -> void:
	if not is_instance_valid(file_list):
		return

	var current_selection := get_selected_file_path()

	file_list.clear()
	filtered_files.clear()

	var filter_text := filter_input.text.to_lower() if is_instance_valid(filter_input) else ""

	var current_index := -1
	var index := 0

	for path: String in files.keys():
		var file_data: Dictionary = files[path]
		var file_name := path.get_file()

		if not filter_text.is_empty() and file_name.to_lower().find(filter_text) == -1:
			continue

		var display_name := file_name
		if file_data.modified:
			display_name += " (*)"

		file_list.add_item(display_name)
		filtered_files.append(path)
		file_list.set_item_tooltip(index, path)

		if path == current_path:
			current_index = index

		if path == current_selection:
			file_list.select(index)

		index += 1

	if current_index >= 0 and (file_list.get_selected_items().is_empty() or current_path != current_selection):
		file_list.select(current_index)

# Signal handlers
func _on_filter_text_changed(_text: String) -> void:
	_update_ui()

func _on_item_selected(index: int) -> void:
	if index >= 0 and index < filtered_files.size():
		file_selected.emit(filtered_files[index])

func _on_item_clicked(index: int, at_position: Vector2, mouse_button_index: int) -> void:
	if index >= 0 and index < filtered_files.size():
		if mouse_button_index == MOUSE_BUTTON_RIGHT:
			file_list.select(index)
			file_context_requested.emit(filtered_files[index], at_position)

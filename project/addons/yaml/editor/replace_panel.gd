@tool
class_name YAMLEditorReplacePanel extends HBoxContainer

signal replace_performed
signal replace_all_performed

@export var editor: YAMLCodeEditor
@export var replace_input: LineEdit
@export var replace_button: Button
@export var replace_all_button: Button
@export var selection_only_checkbox: CheckBox

var find_panel: YAMLEditorFindPanel
var original_selection_start: Vector2i
var original_selection_end: Vector2i
var has_original_selection: bool = false

func _ready() -> void:
	# Connect button signals
	replace_button.pressed.connect(_on_replace_button_pressed)
	replace_all_button.pressed.connect(_on_replace_all_button_pressed)
	selection_only_checkbox.toggled.connect(_on_selection_only_toggled)

	# Initially disable buttons
	replace_button.disabled = true
	replace_all_button.disabled = true

func setup(p_find_panel: YAMLEditorFindPanel) -> void:
	find_panel = p_find_panel

	# Connect find panel signals
	find_panel.find_next_requested.connect(_on_find_next)
	find_panel.find_previous_requested.connect(_on_find_next)
	find_panel.connect("visibility_changed", _on_find_panel_visibility_changed)

func show_panel() -> void:
	if not is_instance_valid(editor) or not is_instance_valid(find_panel):
		return

	visible = true

	# Store original selection if "Selection Only" is checked
	if selection_only_checkbox.button_pressed and editor.has_selection():
		_store_original_selection()

	# Update button states based on find panel's match state
	_update_button_states()

func hide_panel() -> void:
	visible = false
	has_original_selection = false

func _store_original_selection() -> void:
	has_original_selection = true
	original_selection_start = Vector2i(editor.get_selection_from_line(), editor.get_selection_from_column())
	original_selection_end = Vector2i(editor.get_selection_to_line(), editor.get_selection_to_column())

func _is_match_in_selection(match_pos: Vector2i, match_length: int) -> bool:
	if not has_original_selection:
		return true

	# Convert match position to absolute character index
	var match_start_index = _get_absolute_index(match_pos.x, match_pos.y)
	var match_end_index = match_start_index + match_length

	# Convert selection to absolute character index
	var selection_start_index = _get_absolute_index(original_selection_start.x, original_selection_start.y)
	var selection_end_index = _get_absolute_index(original_selection_end.x, original_selection_end.y)

	# Check if match is fully contained in selection
	return match_start_index >= selection_start_index and match_end_index <= selection_end_index

func _get_absolute_index(line: int, column: int) -> int:
	# Calculate absolute character index from line and column
	var index = 0
	for i in range(line):
		index += editor.get_line(i).length() + 1  # +1 for newline

	index += column
	return index

func _on_replace_button_pressed() -> void:
	if not is_instance_valid(find_panel) or not is_instance_valid(editor):
		return

	if find_panel.matches.is_empty() or find_panel.current_match_index < 0:
		return

	# Get current match
	var match_pos = find_panel.matches[find_panel.current_match_index]
	var search_text = find_panel.find_input.text
	var replace_text = replace_input.text

	# Check if match is in selection when "Selection Only" is checked
	if selection_only_checkbox.button_pressed and not _is_match_in_selection(match_pos, search_text.length()):
		# Skip this match and go to next
		find_panel.find_next()
		return

	# Store editor state
	var text = editor.text

	# Replace the text
	var line_text = editor.get_line(match_pos.x)
	var new_line_text = line_text.substr(0, match_pos.y) + replace_text + line_text.substr(match_pos.y + search_text.length())
	editor.set_line(match_pos.x, new_line_text)

	# Update the document's content
	editor.text_changed.emit()

	# Refresh search to find the next match
	find_panel.trigger_search()

	# If there are still matches, select the next one
	if not find_panel.matches.is_empty():
		find_panel._select_current_match()

	replace_performed.emit()

func _on_replace_all_button_pressed() -> void:
	if not is_instance_valid(find_panel) or not is_instance_valid(editor):
		return

	if find_panel.matches.is_empty():
		return

	var search_text = find_panel.find_input.text
	var replace_text = replace_input.text

	# Create a copy of matches to iterate through
	var matches_to_replace = find_panel.matches.duplicate()

	# Sort matches in reverse order (to not affect positions of earlier matches)
	matches_to_replace.sort_custom(func(a, b):
		if a.x == b.x:
			return a.y > b.y
		return a.x > b.x
	)

	# Store original text
	var original_text = editor.text
	var lines = original_text.split("\n", false)
	var replacements_count = 0

	# Process replacements
	for match_pos in matches_to_replace:
		# Check if in selection bounds
		if selection_only_checkbox.button_pressed and not _is_match_in_selection(match_pos, search_text.length()):
			continue

		# Replace text in the line
		var line_text = lines[match_pos.x]
		lines[match_pos.x] = line_text.substr(0, match_pos.y) + replace_text + line_text.substr(match_pos.y + search_text.length())
		replacements_count += 1

	# Only update if we made changes
	if replacements_count > 0:
		# Set the new text
		editor.text = "\n".join(lines)

		# Update the document's content
		editor.text_changed.emit()

		# Refresh search
		find_panel.trigger_search()

	replace_all_performed.emit()

	# Show message in status bar if needed
	# This would require integration with the status panel

func _on_find_panel_visibility_changed() -> void:
	if not find_panel.visible:
		hide_panel()
	_update_button_states()

func _on_find_next() -> void:
	_update_button_states()

func _on_selection_only_toggled(toggled: bool) -> void:
	if toggled and editor.has_selection():
		_store_original_selection()
	else:
		has_original_selection = false

func _update_button_states() -> void:
	var has_matches = is_instance_valid(find_panel) and not find_panel.matches.is_empty()
	replace_button.disabled = not has_matches
	replace_all_button.disabled = not has_matches

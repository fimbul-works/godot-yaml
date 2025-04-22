@tool
class_name YAMLEditorFindReplaceBar extends Control

signal replace_performed
signal replace_all_performed

@export_category("Find Panel Components")
@export var find_input: LineEdit
@export var matches_label: Label
@export var previous_button: Button
@export var next_button: Button
@export var match_case_checkbox: CheckBox
@export var whole_words_checkbox: CheckBox
@export var find_button_container: HBoxContainer
@export var find_options_container: HBoxContainer

@export_category("Replace Panel Components")
@export var replace_input: LineEdit
@export var replace_button: Button
@export var replace_all_button: Button
@export var selection_only_checkbox: CheckBox
@export var replace_button_container: HBoxContainer
@export var replace_options_container: HBoxContainer

@export_category("Visibility Toggle")
@export var hide_button: Button

@export_category("Node References")
@export var editor: YAMLCodeEditor
@export var vbox_container: VBoxContainer  # Container holding both panels
@export var find_panel: Control  # First row (find)
@export var replace_panel: Control  # Second row (replace)

# Matching state
var matches: Array[Vector2i] = []  # Store line/column pairs of matches
var current_match_index: int = -1  # Index of current selection in matches array
var search_regex: RegEx = RegEx.new()

func _ready() -> void:
	# Setup UI
	previous_button.icon = get_theme_icon("MoveUp", "EditorIcons")
	next_button.icon = get_theme_icon("MoveDown", "EditorIcons")
	hide_button.icon = get_theme_icon("Close", "EditorIcons")

	# Connect signals
	find_input.text_changed.connect(_on_find_input_changed)
	find_input.text_submitted.connect(_on_find_input_submitted)
	previous_button.pressed.connect(_on_previous_button_pressed)
	next_button.pressed.connect(_on_next_button_pressed)
	match_case_checkbox.toggled.connect(_on_option_changed)
	whole_words_checkbox.toggled.connect(_on_option_changed)
	hide_button.pressed.connect(_on_hide_button_pressed)

	replace_button.pressed.connect(_on_replace_button_pressed)
	replace_all_button.pressed.connect(_on_replace_all_button_pressed)
	selection_only_checkbox.toggled.connect(_on_option_changed)

	# Disable buttons initially
	previous_button.disabled = true
	next_button.disabled = true
	replace_button.disabled = true
	replace_all_button.disabled = true

	# Hide by default
	visible = false

	# Make sure editor preserves selection when focus changes
	if editor:
		editor.set_deselect_on_focus_loss_enabled(false)

var find_panel_visible: bool:
	get(): return find_input.visible
	set(value):
		find_input.visible = value
		find_button_container.visible = value
		find_options_container.visible = value

var replace_panel_visible: bool:
	get(): return replace_input.visible
	set(value):
		replace_input.visible = value
		replace_button_container.visible = value
		replace_options_container.visible = value

# Public methods
func show_find_panel() -> void:
	if not is_instance_valid(editor):
		return

	visible = true
	find_panel_visible = true

	# If there's a selection, use it as search text
	if editor.has_selection():
		find_input.text = editor.get_selected_text()

	# Run initial search and update UI
	trigger_search()

	# Focus the search input
	find_input.grab_focus()
	find_input.select_all()

func show_replace_panel() -> void:
	if not is_instance_valid(editor):
		return

	visible = true
	find_panel_visible = true
	replace_panel_visible = true

	# If there's a selection, use it as search text
	if editor.has_selection():
		find_input.text = editor.get_selected_text()

	# Run initial search and update UI
	trigger_search()

	# Focus the search input
	find_input.grab_focus()
	find_input.select_all()

func hide_panel() -> void:
	find_panel_visible = false
	replace_panel_visible = false
	visible = false

	# Clear search when hiding
	if is_instance_valid(editor):
		editor.set_search_text("")
		editor.set_search_flags(0)
		editor.queue_redraw()

# Core functionality
func trigger_search() -> void:
	if not is_instance_valid(editor):
		return

	# Store old cursor position to find closest match
	var old_cursor_line = editor.get_caret_line()
	var old_cursor_column = editor.get_caret_column()

	# Update TextEdit search settings for highlighting
	var search_text = find_input.text
	editor.set_search_text(search_text if visible else "")

	var flags = 0
	if match_case_checkbox.button_pressed:
		flags |= TextEdit.SEARCH_MATCH_CASE
	if whole_words_checkbox.button_pressed:
		flags |= TextEdit.SEARCH_WHOLE_WORDS
	editor.set_search_flags(flags)

	# Find all matches
	matches.clear()
	current_match_index = -1

	if search_text.is_empty():
		_update_match_label()
		_update_button_states()
		return

	# Create regex pattern
	_create_search_regex(search_text, match_case_checkbox.button_pressed, whole_words_checkbox.button_pressed)

	# Find all matches using regex
	for line_num in range(editor.get_line_count()):
		var line_text = editor.get_line(line_num)
		var search_results = search_regex.search_all(line_text)

		for result in search_results:
			matches.append(Vector2i(line_num, result.get_start()))

	# Determine which match to select
	if matches.is_empty():
		current_match_index = -1
	else:
		# Find closest match to current cursor position
		var best_distance = -1
		var best_match = 0

		for i in range(matches.size()):
			var pos = matches[i]

			# Check if this match is after cursor
			if pos.x > old_cursor_line or (pos.x == old_cursor_line and pos.y >= old_cursor_column):
				var distance = (pos.x - old_cursor_line) * 1000 + (pos.y - old_cursor_column)
				if best_distance < 0 or distance < best_distance:
					best_distance = distance
					best_match = i

		# If no match after cursor, wrap to first match
		if best_distance < 0:
			current_match_index = 0
		else:
			current_match_index = best_match

	# Always ensure we have a selected match if there are any matches
	if matches.size() > 0 and current_match_index == -1:
		current_match_index = 0

	# Select the current match if appropriate
	if current_match_index >= 0 and not (selection_only_checkbox.button_pressed and replace_panel.visible):
		_select_current_match()

	# Update UI
	_update_match_label()
	_update_button_states()

func find_next() -> void:
	if matches.is_empty() or not is_instance_valid(editor):
		return

	# Don't navigate if selection only is active
	if replace_panel.visible and selection_only_checkbox.button_pressed:
		return

	# Move to next match
	current_match_index = (current_match_index + 1) % matches.size()
	_select_current_match()
	_update_match_label()

func find_previous() -> void:
	if matches.is_empty() or not is_instance_valid(editor):
		return

	# Don't navigate if selection only is active
	if replace_panel.visible and selection_only_checkbox.button_pressed:
		return

	# Move to previous match
	current_match_index = (current_match_index - 1 + matches.size()) % matches.size()
	_select_current_match()
	_update_match_label()

# Helper methods
func _create_search_regex(search_text: String, case_sensitive: bool, whole_words: bool) -> void:
	# Escape special regex characters
	var pattern = ""
	for i in range(search_text.length()):
		var c = search_text[i]
		# Escape regex special characters
		if c in "\\.*+?^$[](){}|":
			pattern += "\\" + c
		else:
			pattern += c

	# Add word boundary anchors if needed
	if whole_words:
		pattern = "\\b%s\\b" % pattern

	if not case_sensitive:
		pattern = "(?i)%s" % pattern

	search_regex = RegEx.new()
	search_regex.compile(pattern)

func _select_current_match() -> void:
	if current_match_index < 0 or current_match_index >= matches.size():
		return

	var match_pos = matches[current_match_index]
	var search_length = find_input.text.length()

	# Select the text
	editor.set_caret_line(match_pos.x)
	editor.set_caret_column(match_pos.y)
	editor.select(match_pos.x, match_pos.y, match_pos.x, match_pos.y + search_length)

	# Center the view
	editor.center_viewport_to_caret()

func _update_match_label() -> void:
	if not visible:
		return

	var count = matches.size()

	matches_label.visible = true

	if count > 0:
		matches_label.modulate = Color.WHITE
		matches_label.text = "%d of %d matches" % [current_match_index + 1, count]
	elif not find_input.text.is_empty():
		matches_label.modulate = EditorInterface.get_editor_settings().get_setting("text_editor/theme/highlighting/brace_mismatch_color")
		matches_label.text = "No matches"
	else:
		matches_label.visible = false
		matches_label.text = ""

func _update_button_states() -> void:
	var has_matches = matches.size() > 0
	var selection_only_active = replace_panel.visible and selection_only_checkbox.button_pressed

	# Disable navigation buttons if selection only is checked
	previous_button.disabled = not has_matches or selection_only_active
	next_button.disabled = not has_matches or selection_only_active

	# Enable/disable replace buttons
	if selection_only_active and editor.has_selection():
		var has_matches_in_selection = get_matches_in_selection().size() > 0
		replace_button.disabled = not has_matches_in_selection
		replace_all_button.disabled = not has_matches_in_selection
	else:
		replace_button.disabled = not has_matches
		replace_all_button.disabled = not has_matches

# Get matches within the current selection when Selection Only is active
func get_matches_in_selection() -> Array[Vector2i]:
	var result: Array[Vector2i] = []

	if not editor.has_selection() or not selection_only_checkbox.button_pressed:
		return matches.duplicate()

	var search_text = find_input.text
	var selection_from_line = editor.get_selection_from_line()
	var selection_from_column = editor.get_selection_from_column()
	var selection_to_line = editor.get_selection_to_line()
	var selection_to_column = editor.get_selection_to_column()

	# Convert selection to absolute character index
	var selection_start_index = _get_absolute_index(selection_from_line, selection_from_column)
	var selection_end_index = _get_absolute_index(selection_to_line, selection_to_column)

	for match_pos in matches:
		# Convert match position to absolute character index
		var match_start_index = _get_absolute_index(match_pos.x, match_pos.y)
		var match_end_index = match_start_index + search_text.length()

		# Check if match is fully contained in selection
		if match_start_index >= selection_start_index and match_end_index <= selection_end_index:
			result.append(match_pos)

	return result

# Get the next match after the cursor that's inside the selection
func get_next_match_in_selection() -> Vector2i:
	if not editor.has_selection() or not selection_only_checkbox.button_pressed:
		return Vector2i(-1, -1)

	var matches_in_selection = get_matches_in_selection()
	if matches_in_selection.is_empty():
		return Vector2i(-1, -1)

	var cursor_line = editor.get_caret_line()
	var cursor_column = editor.get_caret_column()

	# Sort matches by position
	matches_in_selection.sort_custom(func(a, b):
		if a.x == b.x:
			return a.y < b.y
		return a.x < b.x
	)

	# Find the first match after cursor
	for match_pos in matches_in_selection:
		if match_pos.x > cursor_line or (match_pos.x == cursor_line and match_pos.y >= cursor_column):
			return match_pos

	# If no match after cursor, wrap to first match
	return matches_in_selection[0]

func _get_absolute_index(line: int, column: int) -> int:
	# Calculate absolute character index from line and column
	var index = 0
	for i in range(line):
		index += editor.get_line(i).length() + 1  # +1 for newline

	index += column
	return index

# Signal handlers
func _on_find_input_changed(_text: String) -> void:
	trigger_search()

func _on_find_input_submitted(_text: String) -> void:
	find_next()

func _on_option_changed(_toggled: bool) -> void:
	trigger_search()

func _on_previous_button_pressed() -> void:
	find_previous()

func _on_next_button_pressed() -> void:
	find_next()

func _on_hide_button_pressed() -> void:
	hide_panel()

func _on_replace_button_pressed() -> void:
	if not is_instance_valid(editor):
		return

	var search_text = find_input.text
	if search_text.is_empty() or matches.is_empty():
		return

	var replace_text = replace_input.text

	# Different behavior based on Selection Only mode
	if selection_only_checkbox.button_pressed and editor.has_selection():
		# Get next match in selection
		var match_pos = get_next_match_in_selection()
		if match_pos.x < 0:  # No match in selection
			return

		# Replace the text
		var line_text = editor.get_line(match_pos.x)
		var new_line_text = line_text.substr(0, match_pos.y) + replace_text + line_text.substr(match_pos.y + search_text.length())
		editor.set_line(match_pos.x, new_line_text)

		# Move cursor to the end of the replaced text for next find
		var cursor_line = match_pos.x
		var cursor_column = match_pos.y + replace_text.length()

		# Update the document's content
		editor.text_changed.emit()

		# Refresh search
		trigger_search()

		# Restore cursor position for next replacement
		editor.set_caret_line(cursor_line)
		editor.set_caret_column(cursor_column)
	else:
		# Normal replace mode - use the current highlighted match
		if current_match_index < 0 or current_match_index >= matches.size():
			return

		# Get current match
		var match_pos = matches[current_match_index]

		# Replace the text
		var line_text = editor.get_line(match_pos.x)
		var new_line_text = line_text.substr(0, match_pos.y) + replace_text + line_text.substr(match_pos.y + search_text.length())
		editor.set_line(match_pos.x, new_line_text)

		# Update the document's content
		editor.text_changed.emit()

		# Refresh search
		trigger_search()

	replace_performed.emit()

func _on_replace_all_button_pressed() -> void:
	if not is_instance_valid(editor):
		return

	var search_text = find_input.text
	if search_text.is_empty() or matches.is_empty():
		return

	var replace_text = replace_input.text

	# Determine which matches to replace
	var matches_to_replace: Array[Vector2i]

	if selection_only_checkbox.button_pressed and editor.has_selection():
		matches_to_replace = get_matches_in_selection()
	else:
		matches_to_replace = matches.duplicate()

	if matches_to_replace.is_empty():
		return

	# Sort matches in reverse order (to not affect positions of earlier matches)
	matches_to_replace.sort_custom(func(a, b):
		if a.x == b.x:
			return a.y > b.y
		return a.x > b.x
	)

	# Process replacements
	var lines = editor.text.split("\n", false)
	var replacements_count = 0

	for match_pos in matches_to_replace:
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
		trigger_search()

	replace_all_performed.emit()

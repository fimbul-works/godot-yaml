@tool
class_name YAMLEditorFindPanel extends HBoxContainer

signal find_next_requested
signal find_previous_requested

@export var editor: YAMLCodeEditor
@export var find_input: LineEdit
@export var matches_label: Label
@export var previous_button: Button
@export var next_button: Button
@export var match_case_checkbox: CheckBox
@export var whole_words_checkbox: CheckBox
@export var hide_button: Button

var matches: Array[Vector2i] = []  # Store line/column pairs of matches
var current_match_index: int = -1   # Index of current selection in matches array
var search_regex: RegEx = RegEx.new()
var last_match_position: Vector2i = Vector2i(-1, -1)  # Store last valid match position

func _ready() -> void:
	# Setup UI
	previous_button.icon = get_theme_icon("MoveUp", "EditorIcons")
	next_button.icon = get_theme_icon("MoveDown", "EditorIcons")
	hide_button.icon = get_theme_icon("Close", "EditorIcons")

	previous_button.disabled = true
	next_button.disabled = true

	# Connect signals
	find_input.text_changed.connect(_on_find_input_changed)
	find_input.text_submitted.connect(_on_find_input_submitted)
	previous_button.pressed.connect(_on_previous_button_pressed)
	next_button.pressed.connect(_on_next_button_pressed)
	match_case_checkbox.toggled.connect(_on_option_changed)
	whole_words_checkbox.toggled.connect(_on_option_changed)
	hide_button.pressed.connect(_on_hide_button_pressed)

	# Hide by default
	visible = false

func show_panel() -> void:
	if not is_instance_valid(editor):
		return

	visible = true

	# If there's a selection, use it as search text
	if editor.has_selection():
		find_input.text = editor.get_selected_text()

	# Run initial search and update UI
	_perform_search()

	# Focus the search input
	find_input.grab_focus()
	find_input.select_all()

func hide_panel() -> void:
	visible = false

	# Clear search when hiding
	if is_instance_valid(editor):
		editor.set_search_text("")
		editor.set_search_flags(0)
		editor.queue_redraw()

func find_next() -> void:
	if matches.is_empty() or not is_instance_valid(editor):
		return

	# Move to next match
	current_match_index = (current_match_index + 1) % matches.size()
	_select_current_match()
	_update_match_label()

	find_next_requested.emit()

func find_previous() -> void:
	if matches.is_empty() or not is_instance_valid(editor):
		return

	# Move to previous match
	current_match_index = (current_match_index - 1 + matches.size()) % matches.size()
	_select_current_match()
	_update_match_label()

	find_previous_requested.emit()

func _perform_search() -> void:
	if not is_instance_valid(editor):
		return

	# Store old cursor position to find closest match
	var old_cursor_line = editor.get_caret_line()
	var old_cursor_column = editor.get_caret_column()

	# Store current match position if valid (even if the search will give no matches)
	if current_match_index >= 0 and current_match_index < matches.size():
		last_match_position = matches[current_match_index]

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
		_update_ui_state()
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
		# First check if the last match position is still valid
		if last_match_position.x >= 0:
			for i in range(matches.size()):
				if matches[i] == last_match_position:
					current_match_index = i
					break

		# If no existing match found, find the closest match to cursor
		if current_match_index == -1:
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

	# Select the current match
	if current_match_index >= 0:
		_select_current_match()

	# Update UI
	_update_match_label()
	_update_ui_state()

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

	# Update last valid match position
	last_match_position = match_pos

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

	if count > 0:
		matches_label.modulate = Color.WHITE
		matches_label.text = "%d of %d matches" % [current_match_index + 1, count]
	elif not find_input.text.is_empty():
		matches_label.modulate = EditorInterface.get_editor_settings().get_setting("text_editor/theme/highlighting/brace_mismatch_color")
		matches_label.text = "No matches"
	else:
		matches_label.text = ""

func _update_ui_state() -> void:
	var has_matches = matches.size() > 0
	previous_button.disabled = not has_matches
	next_button.disabled = not has_matches

# Signal handlers
func _on_find_input_changed(_text: String) -> void:
	_perform_search()

func _on_find_input_submitted(_text: String) -> void:
	find_next()

func _on_option_changed(_toggled: bool) -> void:
	_perform_search()

func _on_previous_button_pressed() -> void:
	find_previous()

func _on_next_button_pressed() -> void:
	find_next()

func _on_hide_button_pressed() -> void:
	hide_panel()

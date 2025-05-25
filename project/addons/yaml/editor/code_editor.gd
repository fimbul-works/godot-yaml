@tool
class_name YAMLCodeEditor extends CodeEdit

signal content_changed
signal save_requested
signal close_requested
signal undo_requested
signal redo_requested
signal validation_requested
signal zoom_changed(zoom_level)  # New signal for zoom changes

var error_indicators := {}
var snapshot_debounce_timer: Timer
var error_line_color: Color = Color(1.0, 0.3, 0.3, 0.1)
var syntax_highlighter_script = preload("res://addons/yaml/editor/syntax_highlighter.gd")
var suppress_text_changed: bool = false

# Zoom functionality variables
var zoom_level: float = 1.0  # 100%
var default_font_size: int = 14  # Default font size

func _ready() -> void:
	# Clear text to reset the editor state
	text = ""

	# Do not lose selection when focus is lost
	deselect_on_focus_loss_enabled = false
	set_focus_mode(Control.FOCUS_ALL)

	# Configure YAML-specific settings
	set_indent_size(2)
	set_indent_using_spaces(true)
	indent_automatic = true
	scroll_smooth = true
	set_highlight_current_line(true)

	# Set up syntax highlighter
	if not syntax_highlighter:
		syntax_highlighter = syntax_highlighter_script.new()

	# Create debounce timer for content changes
	snapshot_debounce_timer = Timer.new()
	add_child(snapshot_debounce_timer)
	snapshot_debounce_timer.one_shot = true
	snapshot_debounce_timer.wait_time = 0.3  # 300ms
	snapshot_debounce_timer.timeout.connect(_on_snapshot_debounce_timeout)

	# Connect signals
	text_changed.connect(_on_text_changed)
	gui_input.connect(_on_gui_input_focus)

	# Register YAML code completion
	register_yaml_code_completion()

	# Apply initial font size
	_update_font_size()

func _on_text_changed() -> void:
	if suppress_text_changed:
		return

	# Clear error indicators when text changes
	clear_error_indicators()

	# Request a snapshot with debounce
	snapshot_debounce_timer.start()

func _on_snapshot_debounce_timeout() -> void:
	# Emit content changed signal
	content_changed.emit()

	# Request validation
	validation_requested.emit()

func _on_gui_input_focus(event: InputEvent) -> void:
	# Grab focus when clicked
	if event is InputEventMouseButton and event.pressed and event.button_index == MOUSE_BUTTON_LEFT:
		grab_focus()

func cut_selection() -> void:
	if has_selection():
		# Cut the selected text to clipboard
		DisplayServer.clipboard_set(get_selected_text())
		delete_selection()
	else:
		# If no selection, cut the current line (like default script editor)
		var line := get_caret_line()
		var line_text := get_line(line)
		DisplayServer.clipboard_set(line_text)

		# Delete the current line
		select(line, 0, line, line_text.length())
		delete_selection()

		# If this isn't the last line, also remove the line break
		if line < get_line_count() - 1:
			select(line, 0, line + 1, 0)
			delete_selection()

	# Trigger content changed
	text_changed.emit()

func copy_selection() -> void:
	if has_selection():
		# Copy selected text to clipboard
		DisplayServer.clipboard_set(get_selected_text())
	else:
		# If no selection, copy the current line
		var line := get_caret_line()
		var line_text := get_line(line)
		DisplayServer.clipboard_set(line_text)

func paste_clipboard() -> void:
	# Get clipboard content
	var clipboard = DisplayServer.clipboard_get()
	if clipboard.is_empty():
		return

	if has_selection():
		# Replace selected text with clipboard content
		delete_selection()

	# Insert clipboard content at caret position
	insert_text_at_caret(clipboard)
	text_changed.emit()

# Zoom management functions
func zoom_in() -> void:
	zoom_level = min(zoom_level + 0.07, 3.0)  # Max 200%
	_update_font_size()
	zoom_changed.emit(zoom_level)

func zoom_out() -> void:
	zoom_level = max(zoom_level - 0.07, 0.25)  # Min 50%
	_update_font_size()
	zoom_changed.emit(zoom_level)

func zoom_reset() -> void:
	zoom_level = 1.0
	_update_font_size()
	zoom_changed.emit(zoom_level)

func set_zoom(zoom: float) -> void:
	zoom_level = max(0.25, min(3.0, zoom))
	_update_font_size()
	zoom_changed.emit(zoom_level)

func _update_font_size() -> void:
	var new_size = int(default_font_size * zoom_level)
	add_theme_font_size_override("font_size", new_size)

func _unhandled_key_input(event: InputEvent) -> void:
	# Handle tab key before focus system gets it
	if event is InputEventKey and event.pressed and has_focus():
		match event.keycode:
			KEY_TAB:
				if event.shift_pressed:
					# Handle Shift+Tab for unindent
					_handle_unindent()
				else:
					# Handle Tab for indent
					_handle_indent()
				get_viewport().set_input_as_handled()
				return

func _gui_input(event: InputEvent) -> void:
	# Handle shortcuts for saving/closing
	if event is InputEventKey and event.pressed:
		match event.get_keycode_with_modifiers():
			KEY_MASK_CTRL | KEY_S:
				save_requested.emit()
				get_viewport().set_input_as_handled()
			KEY_MASK_CTRL | KEY_W:
				close_requested.emit()
				get_viewport().set_input_as_handled()
			KEY_ENTER, KEY_KP_ENTER:
				# Handle auto-continuation of YAML structures
				_handle_enter_key()
				get_viewport().set_input_as_handled()
			KEY_MASK_CTRL | KEY_Z:
				# Handle undo
				undo_requested.emit()
				get_viewport().set_input_as_handled()
			KEY_MASK_CTRL | KEY_Y, KEY_MASK_CTRL | KEY_MASK_SHIFT | KEY_Z:
				# Handle redo (supports both Ctrl+Y and Ctrl+Shift+Z)
				redo_requested.emit()
				get_viewport().set_input_as_handled()
			KEY_MASK_CTRL | KEY_EQUAL, KEY_MASK_CTRL | KEY_KP_ADD:
				zoom_in()
			KEY_MASK_CTRL | KEY_MINUS, KEY_MASK_CTRL | KEY_KP_SUBTRACT:
				zoom_out()
			KEY_MASK_CTRL | KEY_0, KEY_MASK_CTRL | KEY_KP_0:
				zoom_reset()

func set_text_and_preserve_state(new_text: String, preserve_state: bool = true) -> void:
	if preserve_state:
		# Save current state
		var previous_caret_pos := get_caret_column()
		var previous_line := get_caret_line()
		var previous_scroll_v := get_v_scroll_bar().value
		var previous_scroll_h := get_h_scroll_bar().value

		# Set text without triggering our own text_changed handler
		suppress_text_changed = true
		text = new_text
		suppress_text_changed = false

		# Restore state if possible
		if previous_line < get_line_count():
			set_caret_line(previous_line)
			var line_length := get_line(previous_line).length()
			if previous_caret_pos <= line_length:
				set_caret_column(previous_caret_pos)

		# Restore scroll position (with a small delay to ensure the text is updated first)
		call_deferred("_restore_scroll_position", previous_scroll_v, previous_scroll_h)
	else:
		# Just set the text without preserving state
		suppress_text_changed = true
		text = new_text
		suppress_text_changed = false

func _restore_scroll_position(v_scroll: float, h_scroll: float) -> void:
	# Wait for one frame to ensure the text has been updated and rendered
	if get_tree():
		await get_tree().process_frame
		get_v_scroll_bar().value = v_scroll
		get_h_scroll_bar().value = h_scroll

func _handle_indent() -> void:
	# Get current line and text
	var line := get_caret_line()
	var line_text := get_line(line)

	# Get selection so we can handle multi-line indentation
	var selection_active := has_selection()
	var selection_from := get_selection_from_line()
	var selection_to := get_selection_to_line()

	if selection_active:
		# Indent multiple lines
		begin_complex_operation()
		for i in range(selection_from, selection_to + 1):
			set_line(i, "  " + get_line(i))
		end_complex_operation()
	else:
		# Simple indent - insert 2 spaces at caret position
		insert_text_at_caret("  ")

	# Trigger text changed to update the document
	text_changed.emit()

func _handle_unindent() -> void:
	# Get current line and text
	var line := get_caret_line()
	var text := get_line(line)

	# Get selection so we can handle multi-line unindentation
	var selection_active := has_selection()
	var selection_from := get_selection_from_line()
	var selection_to := get_selection_to_line()

	if selection_active:
		# Unindent multiple lines
		begin_complex_operation()
		for i in range(selection_from, selection_to + 1):
			var line_text := get_line(i)
			if line_text.begins_with("  "):
				set_line(i, line_text.substr(2))
			elif line_text.begins_with(" "):
				set_line(i, line_text.substr(1))
		end_complex_operation()
	else:
		# Simple unindent - remove up to 2 spaces from beginning of line
		if text.begins_with("  "):
			set_line(line, text.substr(2))
			set_caret_column(max(0, get_caret_column() - 2))
		elif text.begins_with(" "):
			set_line(line, text.substr(1))
			set_caret_column(max(0, get_caret_column() - 1))

func _handle_enter_key() -> void:
	var line := get_caret_line()
	var line_text := get_line(line)

	# Auto-continuation for lists
	if "- " in line_text:
		var indent_level := 0
		for c in line_text:
			if c == ' ':
				indent_level += 1
			else:
				break

		# Insert the line with the same indentation and list marker
		var new_line := "\n" + " ".repeat(indent_level) + "- "
		insert_text_at_caret(new_line)
	else:
		# Regular line break with preserved indentation
		var indent_level := 0
		for c in line_text:
			if c == ' ':
				indent_level += 1
			else:
				break

		# Increased indentation if the line ends with a colon
		if line_text.strip_edges().ends_with(":"):
			indent_level += 2

		insert_text_at_caret("\n" + " ".repeat(indent_level))

func register_yaml_code_completion() -> void:
	# Register common YAML keywords and patterns for code completion
	var keyword_list: PackedStringArray = [
		"true",
		"false",
		"null",
		"~",
		"INF",
		"-INF"
	]

	# Add keywords and tags to completion
	for keyword in keyword_list:
		add_code_completion_option(CodeCompletionKind.KIND_CONSTANT, keyword, keyword)

	## YAML tags
	var tag_list: PackedStringArray = [
		"!Resource",
		"!AABB",
		"!Basis",
		"!Color",
		"!NodePath",
		"!PackedByteArray",
		"!PackedColorArray",
		"!PackedFloat32Array",
		"!PackedFloat64Array",
		"!PackedInt32Array",
		"!PackedInt64Array",
		"!PackedStringArray",
		"!PackedVector2Array",
		"!PackedVector3Array",
		"!Plane",
		"!Projection",
		"!Quaternion",
		"!Rect2",
		"!Rect2i",
		"!StringName",
		"!Transform2D",
		"!Transform3D",
		"!Vector2",
		"!Vector2i",
		"!Vector3",
		"!Vector3i",
		"!Vector4",
		"!Vector4i"
	]

	for tag in tag_list:
		add_code_completion_option(CodeCompletionKind.KIND_CLASS, tag, tag)

func mark_error_line(line: int, message: String) -> void:
	if line < 0 or line >= get_line_count():
		return

	# Set line background to error color
	var error_color: Color = EditorInterface.get_editor_settings().get_setting("text_editor/theme/highlighting/mark_color")
	set_line_background_color(line, error_color)

	# Set gutter icon
	var error_icon := get_theme_icon("StatusError", "EditorIcons")
	if error_icon:
		set_line_gutter_icon(line, 0, error_icon)

	# Store for later reference
	error_indicators[line] = message

func clear_error_indicators() -> void:
	for line: int in error_indicators:
		set_line_background_color(line, Color(0, 0, 0, 0))
		set_line_gutter_icon(line, 0, null)

	error_indicators.clear()

func get_current_line_col_info() -> Array[int]:
	var line := get_caret_line() + 1
	var col := get_caret_column() + 1
	return [line, col]

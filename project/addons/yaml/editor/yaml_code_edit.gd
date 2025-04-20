@tool
class_name YAMLCodeEditor extends CodeEdit

signal content_changed
signal snapshot_requested
signal save_requested
signal close_requested
signal undo_requested
signal redo_requested
signal validation_requested

var error_indicators = {}
var snapshot_debounce_timer: Timer
var error_line_color: Color = Color(1.0, 0.3, 0.3, 0.1)
var syntax_highlighter_script = preload("res://addons/yaml/editor/syntax_highlighter.gd")

func _ready() -> void:
	# Clear the text to start with
	text = ""

	# Configure YAML-specific settings
	set_indent_size(2)
	set_indent_using_spaces(true)
	indent_automatic = true
	scroll_smooth = true
	set_highlight_current_line(true)

	# Set up syntax highlighter
	if not syntax_highlighter:
		syntax_highlighter = syntax_highlighter_script.new()

	# Create debounce timer for snapshot requests
	snapshot_debounce_timer = Timer.new()
	snapshot_debounce_timer.one_shot = true
	snapshot_debounce_timer.wait_time = 0.3  # 300ms
	snapshot_debounce_timer.timeout.connect(_on_snapshot_debounce_timeout)
	add_child(snapshot_debounce_timer)

	# Connect signals
	text_changed.connect(_on_text_changed)

	# Register YAML code completion
	register_yaml_code_completion()

func _on_text_changed() -> void:
	# Clear error indicators when text changes
	clear_error_indicators()

	# Emit content changed signal
	content_changed.emit()

	# Request a snapshot with debounce
	snapshot_debounce_timer.start()

	# Request validation
	validation_requested.emit()

func _on_snapshot_debounce_timeout() -> void:
	snapshot_requested.emit()

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
			KEY_TAB:
				# Smart indent for YAML
				_handle_indent()
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

func set_text_and_preserve_state(new_text: String, preserve_state: bool = true) -> void:
	if preserve_state:
		# Save current state
		var previous_caret_pos = get_caret_column()
		var previous_line = get_caret_line()
		var previous_scroll_v = get_v_scroll_bar().value
		var previous_scroll_h = get_h_scroll_bar().value

		# Set text
		text = new_text

		# Restore state if possible
		if previous_line < get_line_count():
			set_caret_line(previous_line)
			var line_length = get_line(previous_line).length()
			if previous_caret_pos <= line_length:
				set_caret_column(previous_caret_pos)

		# Restore scroll position (with a small delay to ensure the text is updated first)
		call_deferred("_restore_scroll_position", previous_scroll_v, previous_scroll_h)
	else:
		# Just set the text without preserving state
		text = new_text

func _restore_scroll_position(v_scroll: float, h_scroll: float) -> void:
	# Wait for one frame to ensure the text has been updated and rendered
	await get_tree().process_frame
	get_v_scroll_bar().value = v_scroll
	get_h_scroll_bar().value = h_scroll

func _handle_indent() -> void:
	# Get current line and text
	var line = get_caret_line()
	var text = get_line(line)

	# Get selection so we can handle multi-line indentation
	var selection_active = has_selection()
	var selection_from = get_selection_from_line()
	var selection_to = get_selection_to_line()

	if selection_active:
		# Indent multiple lines
		begin_complex_operation()
		for i in range(selection_from, selection_to + 1):
			set_line(i, "  " + get_line(i))
		end_complex_operation()
	else:
		# Simple indent
		insert_text_at_caret("  ")

func _handle_enter_key() -> void:
	var line = get_caret_line()
	var text = get_line(line)

	# Auto-continuation for lists
	if "- " in text:
		var indent_level = 0
		for c in text:
			if c == ' ':
				indent_level += 1
			else:
				break

		# Insert the line with the same indentation and list marker
		var new_line = "\n" + " ".repeat(indent_level) + "- "
		insert_text_at_caret(new_line)
	else:
		# Regular line break with preserved indentation
		var indent_level = 0
		for c in text:
			if c == ' ':
				indent_level += 1
			else:
				break

		# Increased indentation if the line ends with a colon
		if text.strip_edges().ends_with(":"):
			indent_level += 2

		insert_text_at_caret("\n" + " ".repeat(indent_level))

func register_yaml_code_completion() -> void:
	# Register common YAML keywords and patterns for code completion
	var keyword_list = [
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
	var tag_list = [
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
	var error_color = get_theme_color("error_color", "Editor") if get_theme_color("error_color", "Editor") else error_line_color
	set_line_background_color(line, error_color)

	# Set gutter icon
	var error_icon = get_theme_icon("StatusError", "EditorIcons")
	if error_icon:
		set_line_gutter_icon(line, 0, error_icon)

	# Store for later reference
	error_indicators[line] = message

func clear_error_indicators() -> void:
	for line in error_indicators:
		set_line_background_color(line, Color(0, 0, 0, 0))
		set_line_gutter_icon(line, 0, null)

	error_indicators.clear()

func get_current_line_col_info() -> Array[int]:
	var line = get_caret_line() + 1
	var col = get_caret_column() + 1
	return [line, col]

func _request_code_completion(force: bool = false) -> void:
	var line = get_caret_line()
	var col = get_caret_column()
	var text = get_line(line).substr(0, col)

	# Auto-complete after special characters
	if text.ends_with(": ") or text.ends_with("- "):
		request_code_completion(force)
	else:
		# Default behavior
		super.request_code_completion(force)

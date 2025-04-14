@tool
extends CodeEdit

# Enhanced CodeEdit with YAML-specific features
var error_indicators = {}
var folded_regions = {}

func _ready() -> void:
	# Configure YAML-specific settings
	set_indent_size(2)
	set_indent_using_spaces(true)

	# Connect signals
	text_changed.connect(_on_text_changed)

	# Set up code intelligence features
	register_yaml_code_completion()

func _on_text_changed() -> void:
	# Automatically clear error indicators when text changes
	clear_error_indicators()

func register_yaml_code_completion() -> void:
	# Register common YAML keywords and patterns for code completion
	var keyword_list = ["true", "false", "null", "~"]

	# Add keywords and tags to completion
	for keyword in keyword_list:
		add_code_completion_option(CodeCompletionKind.KIND_VARIABLE, keyword, keyword)

	## YAML tags
	var tag_list = ["!Resource"]

	for tag in tag_list:
		add_code_completion_option(CodeCompletionKind.KIND_CLASS, tag, tag)

func mark_error_line(line: int, message: String) -> void:
	if line < 0 or line >= get_line_count():
		return

	# Set line background to error color
	var error_color = get_theme_color("error_color", "Editor")
	if error_color == null:
		error_color = Color(1.0, 0.3, 0.3, 0.1)

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

func get_current_indent_level() -> int:
	var line = get_caret_line()
	var text = get_line(line)

	var indent_count = 0
	for c in text:
		if c == ' ':
			indent_count += 1
		elif c == '\t':
			indent_count += get_indent_size()
		else:
			break

	return indent_count / get_indent_size()

func toggle_fold_at_line(line: int) -> void:
	if line < 0 or line >= get_line_count():
		return

	if is_line_folded(line):
		unfold_line(line)
	else:
		fold_line(line)

# Custom YAML folding logic
func find_yaml_block_end(start_line: int) -> int:
	if start_line >= get_line_count() - 1:
		return -1

	var start_indent = get_line_indent_level(start_line)
	var line = start_line + 1

	while line < get_line_count():
		var text = get_line(line).strip_edges(true, false)

		# Skip empty lines and comments
		if text.is_empty() or text.begins_with("#"):
			line += 1
			continue

		var indent = get_line_indent_level(line)
		if indent <= start_indent:
			return line - 1

		line += 1

	return get_line_count() - 1

func get_line_indent_level(line: int) -> int:
	var text = get_line(line)
	var indent_count = 0

	for c in text:
		if c == ' ':
			indent_count += 1
		elif c == '\t':
			indent_count += get_indent_size()
		else:
			break

	return indent_count / get_indent_size()

func _can_fold_line(line: int) -> bool:
	var line_text = get_line(line).strip_edges()

	# Check if the line ends with a colon or has array marker
	return line_text.ends_with(":") or ("-" in line_text.substr(0, 2))

func attempt_fold_yaml_block(line: int) -> void:
	if !_can_fold_line(line):
		return

	var end_line = find_yaml_block_end(line)
	if end_line > line:
		fold_line(line)
		folded_regions[line] = end_line

# Override to add custom indentation behavior
func _on_gui_input(event: InputEvent) -> void:
	if event is InputEventKey and event.pressed:
		match event.keycode:
			KEY_TAB:
				# Smart indentation
				pass
			KEY_ENTER, KEY_KP_ENTER:
				# Auto-continuation of lists
				var current_line = get_caret_line()
				var text = get_line(current_line)

				if text.strip_edges().begins_with("-"):
					# Auto-continue list items with proper indentation
					pass

@tool
extends EditorPlugin

var syntax_highlighter = preload("res://addons/yaml/yaml_syntax_highlighter.gd").new()
var editor: EditorInterface
var script_editor: ScriptEditor
var validation_timer: Timer

func _enter_tree() -> void:
	editor = get_editor_interface()
	script_editor = editor.get_script_editor()

	add_custom_type("YAMLFile", "Resource", null, null)

func _get_code_editor() -> CodeEdit:
	var current_editor = script_editor.get_current_editor()
	if not current_editor:
			return

	return current_editor.get_base_editor()

func _exit_tree() -> void:
	remove_custom_type("YAMLFile")

func _handles(object) -> bool:
	if object is Resource:
		var extension = object.resource_path.get_extension().to_lower()
		return extension in ["yaml", "yml"]
	return false

func _edit(object) -> void:
	if !object:
		return
	var file = FileAccess.open(object.resource_path, FileAccess.READ)
	if not file:
		return
	var content = file.get_as_text()

	var code_editor: CodeEdit = _get_code_editor()
	if !code_editor:
		return

	code_editor.syntax_highlighter = syntax_highlighter
	code_editor.indent_automatic = true
	code_editor.indent_size = 2
	code_editor.indent_use_spaces = true

	if code_editor.text != content:
		code_editor.text = content
		code_editor.set_dirty(false)

	if !code_editor.text_changed.is_connected(_on_text_changed):
		code_editor.text_changed.connect(_on_text_changed)

		validation_timer = Timer.new()
		validation_timer.one_shot = true  # Only trigger once
		validation_timer.wait_time = 0.5  # 500ms delay
		validation_timer.timeout.connect(_validate_yaml)
		code_editor.add_child(validation_timer)

func _on_text_changed():
	# Reset and restart the timer each time text changes
	validation_timer.stop()
	validation_timer.start()

func _validate_yaml():
	var code_editor := _get_code_editor()
	var result = YAML.validate(code_editor.text)

	_clear_background_colors()

	if result.has_error():
		var settings := EditorInterface.get_editor_settings()
		var error_color: Color = settings.get_setting("text_editor/theme/highlighting/mark_color")

		# Add error highlighting
		var line = result.get_error_line()
		var col = result.get_error_column()

		code_editor.set_line_background_color(line, error_color)
		code_editor.set_line_gutter_icon(line, 0, code_editor.get_theme_icon("StatusError", "EditorIcons"))
	else:
		# Clear all error indicators
		_clear_background_colors()
		for i in range(code_editor.get_line_count()):
			code_editor.set_line_gutter_icon(i, 0, null)

func _clear_background_colors() -> void:
	var code_editor := _get_code_editor()
	for line in range(code_editor.get_line_count()):
		code_editor.set_line_background_color(line, Color(0, 0, 0, 0))

func _on_theme_changed():
	syntax_highlighter.clear_highlighting_cache()
	syntax_highlighter.update_highlighting()

func get_plugin_path() -> String:
	return get_script().resource_path.get_base_dir()

func _get_plugin_icon() -> Texture2D:
	return load(get_plugin_path() + "/assets/icon.svg")

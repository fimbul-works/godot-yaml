@tool
extends EditorPlugin

const YAMLEditorPanel = preload("res://addons/yaml/editor/yaml_editor.tscn")

var yaml_editor_instance

func _enter_tree() -> void:
	# Register YAML as a custom resource type
	add_custom_type("YAMLFile", "Resource", null, null)

	# Create the instance exactly as in the example
	yaml_editor_instance = YAMLEditorPanel.instantiate()

	# Pass the editor interface reference to the editor
	yaml_editor_instance.editor = get_editor_interface()

	# Add to the main screen exactly as in the example
	get_editor_interface().get_editor_main_screen().add_child(yaml_editor_instance)

	# Hide initially - this is very important
	_make_visible(false)

func _exit_tree() -> void:
	# Follow exact cleanup pattern
	if yaml_editor_instance:
		yaml_editor_instance.queue_free()

	# Clean up other resources
	remove_custom_type("YAMLFile")

func _has_main_screen() -> bool:
	return true

func _make_visible(visible: bool) -> void:
	if yaml_editor_instance:
		yaml_editor_instance.visible = visible

func _handles(object) -> bool:
	if object is Resource:
		var extension = object.resource_path.get_extension().to_lower()
		return extension in ["yaml", "yml"]
	return false

func _edit(object) -> void:
	if object and yaml_editor_instance:
		_make_visible(true)
		# Only try to call open_file if the method exists
		if yaml_editor_instance.has_method("open_file"):
			yaml_editor_instance.open_file(object.resource_path)

func _get_plugin_name() -> String:
	return "YAML"

func _get_plugin_icon() -> Texture2D:
	return get_editor_interface().get_base_control().get_theme_icon("TextFile", "EditorIcons")

func _on_file_selected(file_path: String) -> void:
	if file_path.get_extension().to_lower() in ["yaml", "yml"]:
		_make_visible(true)
		if yaml_editor_instance.has_method("open_file"):
			yaml_editor_instance.open_file(file_path)

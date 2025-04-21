@tool
extends EditorPlugin

const YAMLEditorPanel = preload("res://addons/yaml/editor/yaml_editor.tscn")
const ShortcutsClass = preload("res://addons/yaml/editor/editor_shortcuts.gd")

var yaml_editor_instance

func _enter_tree() -> void:
	# Initialize YAMLFileSystem singleton
	YAMLFileSystem.get_singleton()

	# Register YAML as a custom resource type
	add_custom_type("YAMLFile", "Resource", null, null)

	# Create the instance
	yaml_editor_instance = YAMLEditorPanel.instantiate()

	# Pass the editor interface reference to the editor
	yaml_editor_instance.editor = get_editor_interface()

	# Add to the main screen
	get_editor_interface().get_editor_main_screen().add_child(yaml_editor_instance)

	# Register keyboard shortcuts
	ShortcutsClass.register_shortcuts(self, yaml_editor_instance)

	# Hide initially - this is very important
	_make_visible(false)

	# Connect file system signals to detect file moves/renames
	get_editor_interface().get_resource_filesystem().resources_reimported.connect(_on_resources_reimported)
	get_editor_interface().get_resource_filesystem().filesystem_changed.connect(_on_filesystem_changed)

	# Also make sure the file system dock's file selection is connected
	# THIS SIGNAL DOES NOT EXISTS
	#get_editor_interface().get_file_system_dock().file_selected.connect(_on_file_selected)

func _exit_tree() -> void:
	# Unregister shortcuts
	ShortcutsClass.unregister_shortcuts()

	# Clean up
	if yaml_editor_instance:
		# Save the current session before closing
		if yaml_editor_instance.session_manager:
			yaml_editor_instance.session_manager.save_session()
		yaml_editor_instance.queue_free()

	# Clean up other resources
	get_editor_interface().get_file_system_dock().file_selected.disconnect(_on_file_selected)
	get_editor_interface().get_resource_filesystem().resources_reimported.disconnect(_on_resources_reimported)
	get_editor_interface().get_resource_filesystem().filesystem_changed.disconnect(_on_filesystem_changed)
	remove_custom_type("YAMLFile")

func _on_filesystem_changed() -> void:
	# Notify the YAML editor that the filesystem has changed
	if yaml_editor_instance and is_instance_valid(yaml_editor_instance):
		yaml_editor_instance.handle_filesystem_change()

func _on_resources_reimported(resources: PackedStringArray) -> void:
	# Check if any of our open YAML files were reimported
	if yaml_editor_instance and is_instance_valid(yaml_editor_instance):
		var file_system = YAMLFileSystem.get_singleton()
		for path in resources:
			if file_system.is_yaml_file(path):
				# Notify the file system singleton about the update
				file_system.notify_file_updated(path)

func _has_main_screen() -> bool:
	return true

func _make_visible(visible: bool) -> void:
	if yaml_editor_instance:
		yaml_editor_instance.visible = visible

		# Focus the code editor when becoming visible
		if visible and is_instance_valid(yaml_editor_instance.code_edit):
			yaml_editor_instance.code_edit.grab_focus()

func _handles(object) -> bool:
	if object is Resource:
		var file_system = YAMLFileSystem.get_singleton()
		return file_system.is_yaml_file(object.resource_path)
	return false

func _edit(object) -> void:
	if object and yaml_editor_instance:
		_make_visible(true)
		yaml_editor_instance.file_manager.open_file(object.resource_path)

func _get_plugin_name() -> String:
	return "YAML"

func get_plugin_path() -> String:
	return get_script().resource_path.get_base_dir()

func _get_plugin_icon() -> Texture2D:
	return load(get_plugin_path() + "/assets/icon.svg")

func _on_file_selected(file_path: String) -> void:
	var file_system = YAMLFileSystem.get_singleton()
	if file_system.is_yaml_file(file_path):
		_make_visible(true)
		yaml_editor_instance.file_manager.open_file(file_path)

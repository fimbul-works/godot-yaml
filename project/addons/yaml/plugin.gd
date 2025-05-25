@tool
extends EditorPlugin

const YAMLEditorPanel = preload("res://addons/yaml/editor/yaml_editor.tscn")
const ShortcutsClass = preload("res://addons/yaml/editor/editor_shortcuts.gd")

var yaml_editor_instance
var resource_loader: YAMLResourceFormat.Loader
var resource_saver: YAMLResourceFormat.Saver

# Store original editor settings to restore on exit
var previous_textfile_extensions: String

func _enter_tree() -> void:
	add_custom_type(
		"YAMLResource", "Resource",
		load("res://addons/yaml/yaml_resource.gd"),
		_get_plugin_icon()
	)

	# Create and register resource format handlers
	resource_loader = YAMLResourceFormat.Loader.new()
	resource_saver = YAMLResourceFormat.Saver.new()

	ResourceLoader.add_resource_format_loader(resource_loader)
	ResourceSaver.add_resource_format_saver(resource_saver)

	# Modify editor settings to remove YAML from text files
	_modify_file_extensions()

	# Initialize YAMLFileSystem singleton and pass editor interface
	var file_system = YAMLFileSystem.get_singleton()
	file_system.set_editor_interface(get_editor_interface())

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

func _exit_tree() -> void:
	# Restore original editor settings
	_restore_file_extensions()

	# Unregister resource format handlers
	if resource_loader:
		ResourceLoader.remove_resource_format_loader(resource_loader)
	if resource_saver:
		ResourceSaver.remove_resource_format_saver(resource_saver)

	remove_custom_type("YAMLResource")

	# Unregister shortcuts
	ShortcutsClass.unregister_shortcuts()

	# Clean up
	if yaml_editor_instance:
		# Save the current session before closing
		if yaml_editor_instance.session_manager:
			yaml_editor_instance.session_manager.save_session()
		yaml_editor_instance.queue_free()

	# Clean up other resources
	get_editor_interface().get_resource_filesystem().resources_reimported.disconnect(_on_resources_reimported)
	get_editor_interface().get_resource_filesystem().filesystem_changed.disconnect(_on_filesystem_changed)

func _modify_file_extensions() -> void:
	var editor_settings: EditorSettings = get_editor_interface().get_editor_settings()

	# Store original settings
	previous_textfile_extensions = editor_settings.get_setting("docks/filesystem/textfile_extensions")

	# Remove yaml/yml from text files (so Script Editor ignores them)
	var new_textfiles = previous_textfile_extensions.replace(",yaml", "").replace(",yml", "").replace("yaml,", "").replace("yml,", "")
	editor_settings.set_setting("docks/filesystem/textfile_extensions", new_textfiles)

func _restore_file_extensions() -> void:
	if previous_textfile_extensions.is_empty():
		return

	var editor_settings: EditorSettings = get_editor_interface().get_editor_settings()
	editor_settings.set_setting("docks/filesystem/textfile_extensions", previous_textfile_extensions)

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
	# Handle YAMLResource (from our resource loader)
	if object is YAMLResource:
		return true

	# Handle Resource objects that are YAML files
	if object is Resource and YAMLFileSystem.get_singleton().is_yaml_file(object.resource_path):
		return true

	# Also check for file paths directly
	if object is String and YAMLFileSystem.get_singleton().is_yaml_file(object):
		return true

	return false

func _edit(object) -> void:
	if object and yaml_editor_instance:
		var file_path: String

		# Handle different object types
		if object is YAMLResource:
			file_path = object.resource_path
		elif object is Resource:
			file_path = object.resource_path
		elif object is String:
			file_path = object
		else:
			return

		# Check if file is already open before trying to open it
		if yaml_editor_instance.file_manager.has_document(file_path):
			# File is already open, just switch to it
			var document = yaml_editor_instance.file_manager.get_document(file_path)
			yaml_editor_instance.file_manager.set_current_document(document)
		else:
			# File is not open, open it normally
			yaml_editor_instance.file_manager.open_file(file_path)

		_make_visible(true)

func _get_plugin_name() -> String:
	return "YAML"

func get_plugin_path() -> String:
	return get_script().resource_path.get_base_dir()

func _get_plugin_icon() -> Texture2D:
	return load(get_plugin_path() + "/icon.svg")

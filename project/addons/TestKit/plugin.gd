@tool
extends EditorPlugin

var engine_version_info := Engine.get_version_info()

func _enter_tree() -> void:
	pass

func _exit_tree() -> void:
	pass

func _get_plugin_name() -> String:
	return "TestKit"

func get_plugin_path() -> String:
	return get_script().resource_path.get_base_dir()

func _get_plugin_icon() -> Texture2D:
	return load(get_plugin_path() + "/icon.svg")

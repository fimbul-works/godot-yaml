@tool
class_name YAMLResourceFormat extends RefCounted

# ResourceFormatLoader for YAML files
class Loader extends ResourceFormatLoader:
	func _get_recognized_extensions() -> PackedStringArray:
		return PackedStringArray(["yaml", "yml"])

	func _get_resource_type(path: String) -> String:
		var ext := path.get_extension().to_lower()
		if YAMLFileSystem.get_singleton().is_yaml_file(path):
			return "YAMLResource"
		return ""

	func _handles_type(type: StringName) -> bool:
		return type == &"YAMLResource"

	func _load(path: String, original_path: String, use_sub_threads: bool, cache_mode: int) -> Variant:
		# Create a lightweight wrapper resource
		var yaml_resource := YAMLResource.new(path)

		# Load the text content
		var error := yaml_resource.load_from_file()
		if error != OK:
			return error

		# Set the resource path so Godot knows where it came from
		yaml_resource.resource_path = path

		return yaml_resource

# ResourceFormatSaver for YAML files
class Saver extends ResourceFormatSaver:
	func _get_recognized_extensions(resource: Resource) -> PackedStringArray:
		if resource is YAMLResource:
			return PackedStringArray(["yaml", "yml"])
		return PackedStringArray()

	func _recognize(resource: Resource) -> bool:
		return resource is YAMLResource

	func _save(resource: Resource, path: String, flags: int) -> Error:
		if not resource is YAMLResource:
			return ERR_INVALID_PARAMETER

		var yaml_resource := resource as YAMLResource

		# Update the file path if it's different
		if yaml_resource.file_path != path:
			yaml_resource.file_path = path
			yaml_resource.resource_path = path

		# Save the text content to file
		return yaml_resource.save_to_file()

extends ExampleBase

# This example requires a local texture reference
# For real use, export a texture in your scene
@export var local_texture: Texture2D

func _init() -> void:
	icon = "🔗"

func run_examples() -> void:
	run_example("Parsing Resources", parsing_resources)
	run_example("Stringifying Resources", stringifying_resources)
	run_example("Resource Security", resource_security)
	run_example("Resource Path Management", resource_path_management)

func parsing_resources() -> void:
	var yaml_text := """
scene: !Resource 'res://addons/yaml/examples/assets/simple_scene.tscn'
texture: !Resource 'res://icon.svg'
yaml: !Resource 'res://addons/yaml/examples/data/simple.yaml'
"""
	log_code_block(yaml_text)

	log_info("Parsing YAML with resource references...")
	var parse_result := YAML.parse(yaml_text)

	if parse_result.has_error():
		log_error("Parse failed: " + parse_result.get_error())
		return

	var data: Dictionary = parse_result.get_data()
	log_success("YAML with resources parsed successfully")

	if data.scene is PackedScene:
		log_success("PackedScene loaded successfully")
		var scene = data.scene.instantiate()
		scene.position = Vector2(640, 360)
		add_child(scene)
	else:
		log_warning("PackedScene not loaded (might be missing file or security restrictions)")

	if data.texture is Texture2D:
		log_success("Texture loaded successfully")
	else:
		log_warning("Texture not loaded (might be missing file or security restrictions)")

	if data.yaml is Dictionary:
		log_success("Nested YAML loaded successfully")
	else:
		log_warning("Nested YAML not loaded (might be missing file or security restrictions)")

	if LOG_VERBOSE:
		log_info("Resource types:")
		log_info("• scene type: " + str(typeof(data.scene)))
		log_info("• texture type: " + str(typeof(data.texture)))
		log_info("YAML resource:")
		log_info(str(data.yaml))

func stringifying_resources() -> void:
	log_info("Stringifying a resource...")

	var resource = load("res://icon.svg")  # Project icon should always exist
	if !resource:
		log_error("Could not load resource")
		return

	var str_result := YAML.stringify(resource)
	if str_result.has_error():
		log_error("Stringify failed: " + str_result.get_error())
		return

	log_success("Resource stringified successfully")

	if LOG_VERBOSE:
		log_result("Stringified Resource:\n" + str_result.get_data())

	# Test stringifying local resource (should fail)
	if local_texture:
		log_info("Attempting to stringify local (non-file) resource...")
		var invalid_result := YAML.stringify(local_texture)

		if invalid_result.has_error():
			log_success("Correctly failed to serialize local resource")
			log_info("Error: " + invalid_result.get_error())
		else:
			log_error("Incorrectly serialized local resource")
	else:
		log_warning("No local texture assigned, skipping local resource test")

func resource_security() -> void:
	log_info("Testing resource security...")

	var yaml_text := """
script: !Resource 'res://addons/yaml/examples/scripts/dangerous_script.gd'
"""
	log_code_block(yaml_text)

	log_info("Parsing with default security...")
	var parse_result := YAML.parse(yaml_text)

	if parse_result.has_error():
		log_success("Default security correctly blocked script resource")
		log_info("Error: " + parse_result.get_error())
	else:
		log_error("Default security failed to block script resource")

	log_info("Creating custom security with explicit type blocks...")
	var security := YAML.create_security()
	security.block_type("Script")
	security.block_type("GDExtension")
	security.allow_path("res://**", ["Texture2D", "PackedScene"])

	log_info("Same file with custom security...")
	parse_result = YAML.parse(yaml_text, security)

	if parse_result.has_error():
		log_success("Custom security correctly blocked script resource")
		log_info("Error: " + parse_result.get_error())
	else:
		log_error("Custom security failed to block script resource")

func resource_path_management() -> void:
	log_subheader("Resource Path Management")

	log_info("Using relative paths vs absolute paths:")

	var yaml_with_absolute_path := """
texture: !Resource 'res://icon.svg'
"""
	log_code_block(yaml_with_absolute_path)

	log_info("Absolute paths are fixed to specific locations")

	var yaml_with_relative_path := """
# When file is in res://addons/yaml/examples/
texture: !Resource '../../../icon.svg'
"""
	log_code_block(yaml_with_relative_path)

	log_warning("Note: Godot doesn't natively support relative paths in Resource paths")
	log_info("You need to implement custom path resolution for relative paths")

	log_info("\nSuggested best practices:")
	log_info("1. Use absolute paths (res://, user://) for resources")
	log_info("2. Keep resources in a structured directory hierarchy")
	log_info("3. Use security settings to limit resource access")
	log_info("4. For mod support, create custom path remapping in your game code")

	# Example of custom path handling
	log_info("\nExample of custom path handling:")
	var code := """
# Custom path resolver for mod resources
func resolve_mod_path(path: String, mod_id: String) -> String:
	if path.begins_with("mod://"):
		# Transform mod:// protocol to user://mods/{mod_id}/
		return path.replace("mod://", "user://mods/" + mod_id + "/")
	return path

# Example usage with YAML
func load_mod_config(mod_id: String) -> Dictionary:
	var yaml_text = FileAccess.open("user://mods/" + mod_id + "/config.yaml", FileAccess.READ).get_as_text()
	var data = YAML.parse(yaml_text).get_data()

	# Process all resource paths in the data
	for key in data:
		if data[key] is String and data[key].begins_with("mod://"):
			# Resolve the path
			data[key] = resolve_mod_path(data[key], mod_id)

	return data
"""
	log_code_block(code)

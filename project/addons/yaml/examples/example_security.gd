extends ExampleBase

func _init() -> void:
	icon = "🔒"

func run_examples() -> void:
	run_example("Default Security Behavior", default_security)
	run_example("Custom Security: Allow Path", custom_security_allow_path)
	run_example("Wildcard Path Patterns", wildcard_paths)
	run_example("Blocking Resource Types", block_type)
	run_example("Clearing Type Restrictions", clear_type_restrictions)
	run_example("Resetting Security", reset_security)
	run_example("Security Best Practices", security_best_practices)

func default_security() -> void:
	log_info("Testing default security settings...")

	# Default security should block Script and GDExtension
	var yaml_text := """
dangerous: !Resource 'res://addons/yaml/examples/classes/my_custom_class.gd'
"""
	log_code_block(yaml_text)

	var result := YAML.parse(yaml_text)

	if result.has_error():
		log_success("Default security correctly blocked Script resource")
		log_info("Error message: " + result.get_error())
		return

	log_error("Security failed to block unsafe resource")

func custom_security_allow_path() -> void:
	log_info("Creating custom security policy...")
	var security := YAML.create_security()

	# Allow only textures from a specific path
	security.allow_path("res://addons/yaml/examples/assets", ["Texture2D"])

	log_info("Testing allowed path and type...")
	var allowed_yaml := """
texture: !Resource 'res://addons/yaml/icon.svg'
"""
	log_code_block(allowed_yaml)

	var result := YAML.parse(allowed_yaml, security)
	if !result.has_error():
		log_success("Correctly allowed texture in permitted path")
	else:
		log_error("Incorrectly blocked permitted resource: " + result.get_error())

	log_info("Testing incorrect path...")
	var wrong_path_yaml := """
texture: !Resource 'res://addons/yaml/examples/wrong_path/test.png'
"""
	log_code_block(wrong_path_yaml)

	result = YAML.parse(wrong_path_yaml, security)
	if result.has_error():
		log_success("Correctly blocked resource outside allowed path")
		log_info("Error message: " + result.get_error())
	else:
		log_error("Security failed to block resource outside allowed path")

	log_info("Testing incorrect type...")
	var wrong_type_yaml := """
scene: !Resource 'res://addons/yaml/examples/assets/textures/test.tscn'
"""
	log_code_block(wrong_type_yaml)

	result = YAML.parse(wrong_type_yaml, security)
	if result.has_error():
		log_success("Correctly blocked non-texture resource")
		log_info("Error message: " + result.get_error())
	else:
		log_error("Security failed to block non-texture resource")

func wildcard_paths() -> void:
	log_info("Testing wildcard path patterns...")

	# Create security configuration
	var security := YAML.create_security()

	# Test single segment wildcard (*)
	log_info("Single segment wildcard (*) example:")
	security.allow_path("res://addons/yaml/*/assets", ["Texture2D"])

	var single_wildcard_yaml := """
texture: !Resource 'res://addons/yaml/icon.svg'
"""
	log_code_block(single_wildcard_yaml)

	var result := YAML.parse(single_wildcard_yaml, security)
	if !result.has_error():
		log_success("Single wildcard pattern matched correctly")
	else:
		log_error("Single wildcard failed: " + result.get_error())

	# Test recursive wildcard (**)
	log_info("\nRecursive wildcard (**) example:")
	security.clear_path_restrictions()
	security.allow_path("res://**", ["PackedScene"])

	var recursive_wildcard_yaml := """
scene: !Resource 'res://addons/yaml/examples/assets/simple_scene.tscn'
"""
	log_code_block(recursive_wildcard_yaml)

	result = YAML.parse(recursive_wildcard_yaml, security)
	if !result.has_error():
		log_success("Recursive wildcard pattern matched correctly")
	else:
		log_error("Recursive wildcard failed: " + result.get_error())

func block_type() -> void:
	log_info("Testing type blocking functionality...")

	var security := YAML.create_security()
	security.allow_path("res://**") # Allow all paths
	security.block_type("PackedScene") # But block all scenes

	var blocked_yaml := """
scene: !Resource 'res://addons/yaml/examples/assets/test.tscn'
"""
	log_code_block(blocked_yaml)

	var result := YAML.parse(blocked_yaml, security)
	if result.has_error():
		log_success("Correctly blocked resource of blocked type")
		log_info("Error message: " + result.get_error())
	else:
		log_error("Failed to block specified resource type")

func clear_type_restrictions() -> void:
	log_info("Testing clearing type restrictions...")

	var security := YAML.create_security()
	security.allow_path("res://**")
	security.clear_type_restrictions() # This removes default blocks on Script and GDExtension

	var script_yaml := """
script: !Resource 'res://addons/yaml/examples/classes/my_custom_class.gd'
"""
	log_code_block(script_yaml)
	log_warning("Note: Allowing scripts can be dangerous with untrusted content")

	var result := YAML.parse(script_yaml, security)
	if !result.has_error():
		log_success("Script resource allowed after clearing restrictions")
	else:
		log_error("Failed to allow script after clearing restrictions: " + result.get_error())

func reset_security() -> void:
	log_info("Testing security reset functionality...")

	var security := YAML.create_security()
	security.allow_path("res://**")
	security.clear_type_restrictions()

	log_info("Before reset: All paths allowed, all types allowed")

	security.reset() # Should revert to default security
	log_info("After reset: Default restrictions should apply")

	var script_yaml := """
script: !Resource 'res://addons/yaml/examples/assets/my_custom_class.gd'
"""
	log_code_block(script_yaml)

	var result := YAML.parse(script_yaml, security)
	if result.has_error():
		log_success("Script correctly blocked after security reset")
		log_info("Error message: " + result.get_error())
	else:
		log_error("Security reset failed to restore default restrictions")

func security_best_practices() -> void:
	log_subheader("Security Best Practices")

	log_info("1. Default security blocks Script and GDExtension resources")
	log_info("2. Only allow specific paths and types needed by your application")
	log_info("3. Use the most specific path patterns possible")
	log_info("4. For user content, create a dedicated directory and apply strict type limitations")

	log_info("\nExample for mod content security:")
	var code := """
# Create security for user mods
var mod_security = YAML.create_security()

# Only allow textures, audio, and text files
mod_security.allow_path("user://mods/**", [
	"Texture2D",
	"CompressedTexture2D",
	"AudioStreamOggVorbis",
	"AudioStreamMP3"
])

# Explicitly block potentially dangerous types
mod_security.block_type("Script")
mod_security.block_type("GDExtension")
mod_security.block_type("PackedScene")

# Use this security when loading mod content
var result = YAML.load_file("user://mods/my_mod/config.yaml", mod_security)
"""
	log_code_block(code)

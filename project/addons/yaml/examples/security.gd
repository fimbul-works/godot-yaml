extends BaseTest

func _init() -> void:
	icon = "🔒"

func test_default_security() -> void:
	# Default security should block Script and GDExtension
	var yaml_text := """
dangerous: !Resource 'res://addons/yaml/examples/classes/my_custom_class.gd'
"""
	var result := YAML.parse(yaml_text)
	expect(result.has_error(), "Default security should block Script resources")
	expect(result.get_error_message().contains("Script"), "Error should mention Script type")

func test_custom_security_allow_path() -> void:
	var security := YAML.create_security()

	# Allow only textures from a specific path
	security.allow_path("res://addons/yaml/examples/assets", ["Texture2D"])

	# This should succeed (texture in allowed path)
	var allowed_yaml := """
texture: !Resource 'res://addons/yaml/examples/assets/texture.png'
"""
	var result := YAML.parse(allowed_yaml, security)
	expect(not result.has_error(), result.get_error_message())

	# This should fail (wrong path)
	var wrong_path_yaml := """
texture: !Resource 'res://addons/yaml/examples/wrong_path/test.png'
"""
	result = YAML.parse(wrong_path_yaml, security)
	expect(result.has_error(), "Should block resources outside allowed paths")

	# This should fail (wrong type)
	var wrong_type_yaml := """
scene: !Resource 'res://addons/yaml/examples/assets/textures/test.tscn'
"""
	result = YAML.parse(wrong_type_yaml, security)
	expect(result.has_error(), "Should block non-texture resources")

func test_wildcard_paths() -> void:
	var security := YAML.create_security()

	# Test single segment wildcard (*)
	security.allow_path("res://addons/yaml/*/assets", ["Texture2D"])

	var single_wildcard_yaml := """
texture: !Resource 'res://addons/yaml/examples/assets/texture.png'
"""
	var result := YAML.parse(single_wildcard_yaml, security)
	expect(not result.has_error(), result.get_error_message())

	# Test recursive wildcard (**)
	security.clear_path_restrictions()
	security.allow_path("res://**", ["PackedScene"])

	var recursive_wildcard_yaml := """
scene: !Resource 'res://addons/yaml/examples/assets/simple_scene.tscn'
"""
	result = YAML.parse(recursive_wildcard_yaml, security)
	expect(not result.has_error(), result.get_error_message())

func test_block_type() -> void:
	var security := YAML.create_security()
	security.allow_path("res://**") # Allow all paths
	security.block_type("PackedScene") # But block all scenes

	var blocked_yaml := """
scene: !Resource 'res://addons/yaml/examples/assets/test.tscn'
"""
	var result := YAML.parse(blocked_yaml, security)
	expect(result.has_error(), "Should block resources of blocked type")

func test_clear_type_restrictions() -> void:
	var security := YAML.create_security()
	security.allow_path("res://**")
	security.clear_type_restrictions() # This removes default blocks on Script and GDExtension

	var script_yaml := """
script: !Resource 'res://addons/yaml/examples/classes/my_custom_class.gd'
"""
	var result := YAML.parse(script_yaml, security)
	print(result)
	expect(not result.has_error(), "Should allow Script resources after clearing restrictions")

func test_reset_security() -> void:
	var security := YAML.create_security()
	security.allow_path("res://**")
	security.clear_type_restrictions()
	security.reset() # Should revert to default security

	var script_yaml := """
script: !Resource 'res://addons/yaml/examples/assets/my_custom_class.gd'
"""
	var result := YAML.parse(script_yaml, security)
	expect(result.has_error(), "Should block Script resources after reset")

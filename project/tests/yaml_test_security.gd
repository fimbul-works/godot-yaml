class_name YAMLSecurityTest extends YAMLTest

# Test resources to use in security tests
var test_resources = {
	"texture": "res://addons/yaml/tests/test_assets/assets/texture.png",
	"material": "res://addons/yaml/tests/test_assets/assets/material.tres",
	"simple_scene": "res://addons/yaml/tests/test_assets/simple_scene.tscn",
	"script_scene": "res://addons/yaml/tests/test_assets/script_scene.tscn",
	"audio": "res://addons/yaml/tests/test_assets/common/audio.wav",
	"font": "res://addons/yaml/tests/test_assets/common/font.ttf",
	"gdscript": "res://addons/yaml/tests/test_assets/scripts/risky_script.gd",
	"gdextension": "res://addons/yaml/yaml.gdextension",
	"custom_resource": "res://addons/yaml/tests/test_assets/assets/custom_resource.tres",
	"user_config": "user://config.tres"
}

func _ready():
	test_name = "YAML Security Tests"
	super._ready()

# Helper function to create YAML with resource paths
func create_resource_yaml(path: String) -> String:
	return "!Resource " + path

# Test default behavior (block scripts and extensions)
func test_default_security():
	# Default security should block Script and GDExtension types
	var script_yaml = create_resource_yaml(test_resources.gdscript)

	var script_result = YAML.parse(script_yaml)
	assert_true(script_result.has_error(), "Default security blocks GDScript")

	var extension_yaml = create_resource_yaml(test_resources.gdextension)
	var extension_result = YAML.parse(extension_yaml)
	assert_true(extension_result.has_error(), "Default security blocks GDExtension")

	# By default with no paths defined, non-blocked resources should be allowed
	var texture_yaml = create_resource_yaml(test_resources.texture)
	var texture_result = YAML.parse(texture_yaml)
	assert_false(texture_result.has_error(), "Default security allows Texture2D")

	var audio_yaml = create_resource_yaml(test_resources.audio)
	var audio_result = YAML.parse(audio_yaml)
	assert_false(audio_result.has_error(), "Default security allows AudioStream")

# Test path-based security with no type restrictions
func test_path_security():
	# Create a custom security instance
	var security = YAML.create_security()

	# Allow only assets path
	security.allow_path("res://addons/yaml/tests/test_assets/assets/")

	# Resources in allowed path should work (except globally blocked)
	var texture_yaml = create_resource_yaml(test_resources.texture)
	var texture_result = YAML.parse(texture_yaml, security)
	assert_false(texture_result.has_error(), "Resources in allowed path are permitted")

	var material_yaml = create_resource_yaml(test_resources.material)
	var material_result = YAML.parse(material_yaml, security)
	assert_false(material_result.has_error(), "Materials in allowed path are permitted")

	# Resources outside allowed path should be blocked
	var audio_yaml = create_resource_yaml(test_resources.audio)
	var audio_result = YAML.parse(audio_yaml, security)
	assert_true(audio_result.has_error(), "Resources outside allowed paths are blocked")

	var font_yaml = create_resource_yaml(test_resources.font)
	var font_result = YAML.parse(font_yaml, security)
	assert_true(font_result.has_error(), "Resources outside allowed paths are blocked")

	# Globally blocked types are still blocked even in allowed paths
	var script_yaml = create_resource_yaml(test_resources.gdscript)
	var script_result = YAML.parse(script_yaml, security)
	assert_true(script_result.has_error(), "Globally blocked types remain blocked even in allowed paths")

# Test path-based security with type restrictions
func test_path_with_types():
	# Create a custom security instance
	var security = YAML.create_security()

	# Allow only Texture2D in assets path
	security.allow_path("res://addons/yaml/tests/test_assets/assets/", ["Texture2D"])

	# Only specified types in allowed path should work
	var texture_yaml = create_resource_yaml(test_resources.texture)
	var texture_result = YAML.parse(texture_yaml, security)
	assert_false(texture_result.has_error(), "Specified type in allowed path is permitted")

	# Other types in the same path should be blocked
	var material_yaml = create_resource_yaml(test_resources.material)
	var material_result = YAML.parse(material_yaml, security)
	assert_true(material_result.has_error(), "Non-specified type in allowed path is blocked")

	# Resources outside allowed path should be blocked
	var audio_yaml = create_resource_yaml(test_resources.audio)
	var audio_result = YAML.parse(audio_yaml, security)
	assert_true(audio_result.has_error(), "Resources outside allowed paths are blocked")

# Test allowing previously blocked types
func test_unblock_types():
	# Create a custom security instance
	var security = YAML.create_security()

	# Clear all type restrictions (including default ones)
	security.clear_type_restrictions()

	# Re-add the path but allow all types (including scripts)
	security.allow_path("res://addons/yaml/tests/test_assets/")

	# Block GDExtension but allow Script
	security.block_type("GDExtension")

	# Script should now be allowed in the path
	var script_yaml = create_resource_yaml(test_resources.gdscript)
	var script_result = YAML.parse(script_yaml, security)
	assert_false(script_result.has_error(), "Unblocked Script type is now allowed in path")

	# GDExtension should still be blocked
	var extension_yaml = create_resource_yaml(test_resources.gdextension)
	var extension_result = YAML.parse(extension_yaml, security)
	assert_true(extension_result.has_error(), "GDExtension remains blocked")

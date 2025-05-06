extends BaseTest

@export var local_texture: Texture2D

func _init() -> void:
	icon = "🔗"

func test_parsing_resources() -> void:
	var yaml_text := """
scene: !Resource 'res://tests/test_assets/simple_scene.tscn'
texture: !Resource 'res://icon.svg'
"""
	var parse_result := YAML.parse(yaml_text)
	expect(!parse_result.has_error(), parse_result.get_error())

	var data: Dictionary = parse_result.get_data()

	if LOG_VERBOSE:
		print_rich("[b]PackedScene[/b] loaded successfully: %s" % [data.scene is PackedScene])
		print_rich("[b]Texture[/b] loaded successfully: %s" % [data.texture is CompressedTexture2D])

	var node = data.scene.instantiate()
	add_child(node)

func test_stringifying_resources() -> void:
	var str_result := YAML.stringify(load("res://addons/yaml/assets/icon.svg"))
	expect(!str_result.has_error(), str_result.get_error_message())

	if LOG_VERBOSE:
		print_rich("[b]Stringified Resource:[/b] %s" % str_result.get_data())

	var invalid_result := YAML.stringify(local_texture)
	expect_equal(invalid_result.get_error_message(), "Cannot serialize local Resource")

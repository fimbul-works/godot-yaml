extends TestSuite

@export var local_texture: Texture2D

func _init() -> void:
	icon = "🔗"

func test_parsing_resources() -> void:
	var yaml_text := """
scene: !Resource 'res://addons/yaml/examples/assets/simple_scene.tscn'
texture: !Resource 'res://icon.svg'
YAML: !Resource 'res://addons/yaml/examples/data/simple.yaml'
"""
	var parse_result := YAML.parse(yaml_text)
	expect(!parse_result.has_error(), parse_result.get_error())

	var data = parse_result.get_data()

	if LOG_VERBOSE:
		print_rich("[b]PackedScene[/b] loaded successfully: %s" % [data.scene is PackedScene])
		print_rich("[b]Texture[/b] loaded successfully: %s" % [data.texture is CompressedTexture2D])

func test_stringifying_resources() -> void:
	var str_result := YAML.stringify(load("res://addons/yaml/icon.svg"))
	expect(!str_result.has_error(), str_result.get_error())

	if LOG_VERBOSE:
		print_rich("[b]Stringified Resource:[/b] %s" % str_result.get_data())

	var invalid_result := YAML.stringify(local_texture)
	expect_equal(invalid_result.get_error(), "Cannot serialize local Resource")

func test_cyclical_references() -> void:
	var yaml_text := """
YAML: !Resource 'res://tests/data/cyclical_a.yaml'
"""
	var parse_result := YAML.parse(yaml_text)
	expect(parse_result.has_error(), "Parser should detect cyclical references")

extends Node2D
## Example showing Resource loading

@export var local_texture: Texture2D

func _ready() -> void:
	if !visible:
		return
	print_rich("[b]🗂️ Resource Referencing[/b]")
	test_parsing_resources()
	test_stringifying_resources()

## Load arbitrary Resource types through YAML reference
func test_parsing_resources() -> void:
	var yaml_text := """
scene: !Resource 'res://addons/yaml/examples/packed_scene.tscn'
texture: !Resource 'res://icon.svg'
"""
	var parse_result := YAML.parse(yaml_text)
	assert(not parse_result.get_error_message(), parse_result.get_error_message())

	var data: Dictionary = parse_result.get_data()

	print_rich("[b]PackedScene[/b] loaded successfully: %s" % [data.scene is PackedScene])
	print_rich("[b]Texture[/b] loaded successfully: %s" % [data.texture is CompressedTexture2D])

	var node = data.scene.instantiate()
	add_child(node)

## Stringify non-local Resource types
func test_stringifying_resources() -> void:
	# Stringify successfully
	var str_result := YAML.stringify(load("res://addons/yaml/assets/icon.svg"))
	assert(not str_result.has_error(), str_result.get_error_message())
	print_rich("[b]Stringified Resource:[/b] %s" % str_result.get_data())

	# Attempt to stringify local resource
	var invalid_result := YAML.stringify(local_texture)
	assert(invalid_result.has_error(), "Unexpected success")

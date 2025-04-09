extends Node2D
## Example showing Resource loading

var yaml_text = """
scene: !Resource 'res://addons/yaml/examples/packed_scene.tscn'
texture: !Resource 'res://icon.svg'
"""

func _ready() -> void:
	if !visible:
		return
	print_rich("[b]🗂️ Resource Referencing[/b]")

	var parse_result = YAML.parse(yaml_text)
	assert(!parse_result.get_error_message(), parse_result.get_error_message())

	var data = parse_result.get_data()

	print_rich("PackedScene loaded successfully: %s" % [data.scene is PackedScene])
	print_rich("Texture loaded successfully: %s" % [data.texture is CompressedTexture2D])

	var node = data.scene.instantiate()
	add_child(node)

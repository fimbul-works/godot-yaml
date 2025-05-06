extends BaseTest

func _init() -> void:
	icon = "📐"

func test_style_extraction() -> void:
	var result = YAML.load_file("res://addons/yaml/data/comprehensive.yaml", null, true)
	expect(!result.has_error(), "Parsing should work")

	var style = result.get_style()
	var tree = style.serialize_style(false)
	if tree.has_error():
		push_error(tree.get_error())
	print(tree.get_data())

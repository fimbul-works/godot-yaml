extends BaseTest

func _init() -> void:
	icon = "📐"

func test_style_extraction() -> void:
	var result = YAML.load_file("res://addons/yaml/data/comprehensive.yaml", null, true)
	expect(!result.has_error(), "Parsing should work")

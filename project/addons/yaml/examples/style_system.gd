extends BaseTest

const YAML_FILE = "res://addons/yaml/data/comprehensive.yaml"

var data
var style

func _init() -> void:
	icon = "📐"

func test_style_extraction() -> void:
	var result = YAML.load_file(YAML_FILE, null, true)
	expect(not result.has_error(), result.get_error())

	data = result.get_data()
	style = result.get_style()
	if LOG_VERBOSE:
		print_rich("\n[b]Extracted Styles:[/b]\n%s" % style.get_debug_string())

func test_stringify_with_style() -> void:
	var result = YAML.stringify(data, style)
	expect(not result.has_error(), result.get_error())

	var yaml_text = result.get_data()
	if LOG_VERBOSE:
		print_rich("\n[b]Stringify with YAMLStyle:[/b]\n%s" % result.get_data())

extends BaseTest

const YAML_FILE = "res://addons/yaml/data/comprehensive.yaml"
const USER_FILE = "user://test.yaml"

var data

func _init() -> void:
	icon = "📁"

func test_validate_file() -> void:
	var result := YAML.validate_file(YAML_FILE)
	expect(!result.has_error(), result.get_error_message())

	if LOG_VERBOSE:
		print("Validation passed")

func test_load_file() -> void:
	var result := YAML.load_file(YAML_FILE)
	expect(!result.has_error(), result.get_error_message())
	data = result.get_data()

	if LOG_VERBOSE:
		var data := result.get_data()
		print_rich("\n[b]Parse Result:[/b]\n%s" % data)

func test_save_file() -> void:
	var result := YAML.save_file(data, USER_FILE)
	expect(!result.has_error(), result.get_error_message())

	if LOG_VERBOSE:
		var yaml := result.get_data()
		print_rich("\n[b]Stringify Result:[/b]\n%s" % yaml)

func test_load_saved_file() -> void:
	var result := YAML.load_file(YAML_FILE)
	expect(!result.has_error(), result.get_error_message())

	if LOG_VERBOSE:
		print_rich("\n[b]Parse Result:[/b]\n%s" % result.get_data())

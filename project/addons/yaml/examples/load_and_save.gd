extends BaseTest

const YAML_FILE = "res://addons/yaml/data/comprehensive.yaml"
const USER_FILE = "user://test.yaml"

var data

func _init() -> void:
	icon = "📁"

func test_validate_file() -> void:
	var result := YAML.validate_file(YAML_FILE)
	expect(not result.has_error(), result.get_error_message())

func test_load_file() -> void:
	var result := YAML.load_file(YAML_FILE)
	expect(not result.has_error(), result.get_error_message())

	data = result.get_data()

	if LOG_VERBOSE:
		print_rich("\n[b]%s Contents:[/b]\n%s\n" % [YAML_FILE, data])

func test_save_file() -> void:
	var result := YAML.save_file(data, USER_FILE)
	expect(not result.has_error(), result.get_error_message())

	if LOG_VERBOSE:
		print_rich("\n[b]Saved YAML to %s:[/b]\n%s\n" % [USER_FILE, result.get_data()])

func test_load_saved_file() -> void:
	var result := YAML.load_file(USER_FILE)
	expect(not result.has_error(), result.get_error_message())

	if LOG_VERBOSE:
		print_rich("\n[b]%s Contents:[/b]\n%s" % [USER_FILE, result.get_data()])

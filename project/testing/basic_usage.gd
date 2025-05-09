extends BaseTest

const YAML_FILE = "res://addons/yaml/data/supported_syntax.yaml"
const USER_FILE = "user://test.yaml"

var yaml_text := """
string: string_value
number: 1234
list:
  - apples
  - oranges
"""

var data

func _init() -> void:
	icon = "✅"

func test_validate_string() -> void:
	var result := YAML.validate(yaml_text)
	expect(not result.has_error(), result.get_error_message())

	if result.has_error():
		return push_error(result.get_error())

func test_parse_text() -> void:
	var result := YAML.parse(yaml_text)
	expect(not result.has_error(), result.get_error_message())

	if result.has_error():
		return push_error(result.get_error())

	if LOG_VERBOSE:
		print_rich("\n[b]Parse Result:[/b]\n%s\n" % result.get_data())

func test_stringify_data() -> void:
	var data = YAML.parse(yaml_text).get_data()

	var result := YAML.stringify(data)
	expect(not result.has_error(), result.get_error_message())

	if result.has_error():
		return push_error(result.get_error())

	expect_equal(result.get_data().strip_edges(), yaml_text.strip_edges(),
		"Stringified YAML should match original")

	if LOG_VERBOSE:
		print_rich("\n[b]Stringify Result:[/b]\n%s\n" % result.get_data())

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

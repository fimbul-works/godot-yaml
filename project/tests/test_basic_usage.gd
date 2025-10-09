extends TestSuite

const YAML_FILE = "res://addons/yaml/examples/data/supported_syntax.yaml"
const USER_FILE = "user://test.yaml"

var yaml_text := """
string: string_value
empty_string: ""
number: 1234
hex: 0xff
list:
  - apples
  - oranges
"""

var data

func _init() -> void:
	icon = "✅"

func test_validate_string() -> void:
	var result := YAML.validate_syntax(yaml_text)
	expect(not result.has_error(), result.get_error())

	if result.has_error():
		return push_error(result.get_error())

func test_parse_text() -> void:
	var result := YAML.parse(yaml_text)
	expect(not result.has_error(), result.get_error())

	if result.has_error():
		return push_error(result.get_error())

	if LOG_VERBOSE:
		print_rich("\n[b]Parse Result:[/b]\n%s\n" % result.get_data())

func test_stringify_data() -> void:
	var parse_result := YAML.parse(yaml_text, null, true)
	if parse_result.has_error():
		return push_error(parse_result.get_error())

	var data = parse_result.get_data()

	var stringify_result := YAML.stringify(data, parse_result.get_style())
	expect(not stringify_result.has_error(), stringify_result.get_error())

	if stringify_result.has_error():
		return push_error(stringify_result.get_error())

	expect_equal(stringify_result.get_data().strip_edges(), yaml_text.strip_edges(),
		"Stringified YAML should match original")

	if LOG_VERBOSE:
		print_rich("\n[b]Stringify Result:[/b]\n%s\n" % stringify_result.get_data())

	var empty_stringify_result = YAML.stringify("")
	expect(not empty_stringify_result.has_error(), empty_stringify_result.get_error())

	var empty_parsed_result = YAML.parse(empty_stringify_result.get_data())
	expect(not empty_parsed_result.has_error(), empty_parsed_result.get_error())
	expect_equal(empty_parsed_result.get_data(), "", "Empty string should be parsed correctly")

func test_validate_file_syntax() -> void:
	var result := YAML.validate_file_syntax(YAML_FILE)
	expect(not result.has_error(), result.get_error())

func test_load_file() -> void:
	var result := YAML.load_file(YAML_FILE)
	expect(not result.has_error(), result.get_error())

	data = result.get_data()

	if LOG_VERBOSE:
		print_rich("\n[b]%s Contents:[/b]\n%s\n" % [YAML_FILE, data])

func test_save_file() -> void:
	var result := YAML.save_file(data, USER_FILE)
	expect(not result.has_error(), result.get_error())

	if LOG_VERBOSE:
		print_rich("\n[b]Saved YAML to %s:[/b]\n%s\n" % [USER_FILE, result.get_data()])

func test_load_saved_file() -> void:
	var result := YAML.load_file(USER_FILE)
	expect(not result.has_error(), result.get_error())

	if LOG_VERBOSE:
		print_rich("\n[b]%s Contents:[/b]\n%s" % [USER_FILE, result.get_data()])

func test_try_methods() -> void:
	# Test try_parse
	var parsed = YAML.try_parse("key: value")
	expect(parsed != null, "try_parse should return data on success")
	expect_equal(parsed.key, "value", "Parsed data should be correct")

	var failed_parse = YAML.try_parse("invalid: [yaml")
	expect_equal(failed_parse, null, "try_parse should return null on error")

	# Test try_stringify
	var stringified = YAML.try_stringify({"test": "data"})
	expect(stringified.length() > 0, "try_stringify should return string on success")

	# Test with circular reference (should return empty string)
	var circular = {}
	circular["self"] = circular
	var failed_stringify = YAML.try_stringify(circular)
	expect_equal(failed_stringify, "", "try_stringify should return empty string on error")

func test_try_file_operations() -> void:
	var test_data = {"file_test": "data"}
	var test_path = "user://test_try_methods.yaml"

	# Test try_save_file
	var save_success = YAML.try_save_file(test_data, test_path)
	expect(save_success, "try_save_file should return true on success")

	var invalid_save = YAML.try_save_file(null, "")
	expect(!invalid_save, "try_save_file should return false on error")

	# Test try_load_file
	var loaded_data = YAML.try_load_file(test_path)
	expect(loaded_data != null, "try_load_file should return data on success")
	expect_equal(loaded_data.file_test, "data", "Loaded data should match")

	var failed_load = YAML.try_load_file("non_existent_file.yaml")
	expect_equal(failed_load, null, "try_load_file should return null on error")

func test_version() -> void:
	var version = YAML.version()
	expect(version.length() > 0, "Version should not be empty")
	expect(version.contains("."), "Version should contain dots")

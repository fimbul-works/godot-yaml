extends BaseTest

var yaml_text := """
string: string_value
number: 1234
list:
  - apples
  - oranges
"""

func _init() -> void:
	icon = "✅"

func test_validate_string() -> void:
	var result := YAML.validate(yaml_text)
	expect(!result.has_error(), result.get_error_message())

func test_parse_string() -> void:
	var result := YAML.parse(yaml_text)
	expect(!result.has_error(), result.get_error_message())

	if LOG_VERBOSE:
		var data := result.get_data()
		print_rich("\n[b]Parse Result:[/b]\n%s" % data)

func test_stringify_data() -> void:
	var yaml_data = YAML.parse(yaml_text).get_data()

	var result := YAML.stringify(yaml_data)
	expect(!result.has_error(), result.get_error_message())

	# Try using the new expect_equal function
	expect_equal(result.get_data().strip_edges(), yaml_text.strip_edges(),
		"Stringified YAML should match original")

	if LOG_VERBOSE:
		var yaml := result.get_data()
		print_rich("\n[b]Stringify Result:[/b]\n%s" % yaml)

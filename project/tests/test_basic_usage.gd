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

	var parsed_data = parse_result.get_data()

	var stringify_result := YAML.stringify(parsed_data, parse_result.get_style())
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

func test_empty_string_round_trip() -> void:
	"""Test that empty strings are properly quoted and round-trip correctly"""
	var test_cases = {
		"empty": "",
		"nested": {"inner_empty": ""},
		"array": ["", "not_empty", ""],
	}

	for key in test_cases.keys():
		var stringify_result = YAML.stringify(test_cases[key])
		expect(not stringify_result.has_error(), "Failed to stringify %s: %s" % [key, stringify_result.get_error()])

		if stringify_result.has_error():
			continue

		var yaml_output = stringify_result.get_data()
		expect(yaml_output.contains('""') or yaml_output.contains("''"),
			"Empty string should be quoted in output for %s: %s" % [key, yaml_output])

		var parse_result = YAML.parse(yaml_output)
		expect(not parse_result.has_error(), "Failed to parse %s: %s" % [key, parse_result.get_error()])

		if parse_result.has_error():
			continue

		var parsed_data = parse_result.get_data()
		expect_equal(parsed_data, test_cases[key],
			"Empty string round-trip failed for %s" % key)

func test_special_yaml_values() -> void:
	"""Test strings that look like YAML special values are properly quoted"""
	var yaml_keywords = {
		"bool_true": "true",
		"bool_false": "false",
		"bool_yes": "yes",
		"bool_no": "no",
		"bool_on": "on",
		"bool_off": "off",
		"null_value": "null",
		"tilde": "~",
	}

	var stringify_result = YAML.stringify(yaml_keywords)
	expect(not stringify_result.has_error(), stringify_result.get_error())

	if stringify_result.has_error():
		return

	var yaml_output = stringify_result.get_data()

	# Parse it back
	var parse_result = YAML.parse(yaml_output)
	expect(not parse_result.has_error(), parse_result.get_error())

	if parse_result.has_error():
		return

	var parsed_data = parse_result.get_data()

	# Verify all values are still strings, not converted to bool/null
	for key in yaml_keywords.keys():
		expect(parsed_data.has(key), "Missing key: %s" % key)
		expect_equal(typeof(parsed_data[key]), TYPE_STRING,
			"Value for %s should be string, got %s" % [key, typeof(parsed_data[key])])
		expect_equal(parsed_data[key], yaml_keywords[key],
			"Value mismatch for %s: expected '%s', got '%s'" % [key, yaml_keywords[key], parsed_data[key]])

func test_numeric_strings() -> void:
	# Test strings that look like numbers are properly quoted
	var numeric_strings = {
		"integer": "12345",
		"float": "123.45",
		"negative": "-42",
		"scientific": "1.23e10",
		"hex": "0xff",
		"octal": "0o77",
		"binary": "0b1010",
		"leading_zero": "0123",
	}

	var stringify_result = YAML.stringify(numeric_strings)
	expect(not stringify_result.has_error(), stringify_result.get_error())

	if stringify_result.has_error():
		return

	var parse_result = YAML.parse(stringify_result.get_data())
	expect(not parse_result.has_error(), parse_result.get_error())

	if parse_result.has_error():
		return

	var parsed_data = parse_result.get_data()

	# Verify all values remain as strings
	for key in numeric_strings.keys():
		expect_equal(typeof(parsed_data[key]), TYPE_STRING,
			"Value for %s should be string, got %s" % [key, typeof(parsed_data[key])])
		expect_equal(parsed_data[key], numeric_strings[key],
			"String value changed for %s" % key)

func test_whitespace_strings() -> void:
	# Test strings with leading/trailing whitespace are properly quotd
	var whitespace_strings = {
		"leading_space": " leading",
		"trailing_space": "trailing ",
		"both_spaces": " both ",
		"leading_tab": "\tleading",
		"trailing_tab": "trailing\t",
		"only_spaces": "   ",
		"newline_in_middle": "line1\nline2",
	}

	var stringify_result = YAML.stringify(whitespace_strings)
	expect(not stringify_result.has_error(), stringify_result.get_error())

	if stringify_result.has_error():
		return

	var parse_result = YAML.parse(stringify_result.get_data())
	expect(not parse_result.has_error(), parse_result.get_error())

	if parse_result.has_error():
		return

	var parsed_data = parse_result.get_data()

	# Verify whitespace is preserved
	for key in whitespace_strings.keys():
		expect_equal(parsed_data[key], whitespace_strings[key],
			"Whitespace not preserved for %s: expected '%s', got '%s'" %
			[key, whitespace_strings[key], parsed_data[key]])

func test_special_characters() -> void:
	"""Test strings with special YAML characters are properly quoted"""
	var special_chars = {
		"colon": "key: value",
		"hash": "#comment",
		"dash": "- item",
		"bracket": "[array]",
		"brace": "{dict}",
		"pipe": "foo|bar",
		"ampersand": "foo&bar",
		"asterisk": "*reference",
		"question": "?key",
		"exclamation": "!tag",
		"percent": "50%",
		"at": "@username",
		"greater": ">folded",
		"less": "<xml>",
		"equals": "a=b",
	}

	var stringify_result = YAML.stringify(special_chars)
	expect(not stringify_result.has_error(), stringify_result.get_error())

	if stringify_result.has_error():
		return

	var parse_result = YAML.parse(stringify_result.get_data())
	expect(not parse_result.has_error(), parse_result.get_error())

	if parse_result.has_error():
		return

	var parsed_data = parse_result.get_data()

	# Verify special characters are preserved
	for key in special_chars.keys():
		expect_equal(parsed_data[key], special_chars[key],
			"Special character not preserved for %s" % key)

func test_multiline_strings() -> void:
	"""Test multiline strings use appropriate block style"""
	var multiline_text = "This is line 1\nThis is line 2\nThis is line 3"
	var multiline_with_quotes = "Text with \"quotes\"\nAnd newlines"

	var test_data = {
		"multiline": multiline_text,
		"multiline_quoted": multiline_with_quotes,
	}

	var stringify_result = YAML.stringify(test_data)
	expect(not stringify_result.has_error(), stringify_result.get_error())

	if stringify_result.has_error():
		return

	var yaml_output = stringify_result.get_data()

	# Multiline strings should use block style (| or >)
	expect(yaml_output.contains("|") or yaml_output.contains(">"),
		"Multiline strings should use block style")

	var parse_result = YAML.parse(yaml_output)
	expect(not parse_result.has_error(), parse_result.get_error())

	if parse_result.has_error():
		return

	var parsed_data = parse_result.get_data()
	expect_equal(parsed_data.multiline, multiline_text,
		"Multiline text not preserved")
	expect_equal(parsed_data.multiline_quoted, multiline_with_quotes,
		"Multiline text with quotes not preserved")

func test_edge_case_strings() -> void:
	"""Test various edge case strings"""
	var edge_cases = {
		"just_dash": "-",
		"just_colon": ":",
		"dots": "...",
		"dashes": "---",
		"mixed": "- : [] {}",
		"url": "https://example.com",
		"path": "/path/to/file",
		"version": "1.2.3",
		"date_like": "2024-01-15",
		"time_like": "12:30:45",
	}

	var stringify_result = YAML.stringify(edge_cases)
	expect(not stringify_result.has_error(), stringify_result.get_error())

	if stringify_result.has_error():
		return

	var parse_result = YAML.parse(stringify_result.get_data())
	expect(not parse_result.has_error(), parse_result.get_error())

	if parse_result.has_error():
		return

	var parsed_data = parse_result.get_data()

	for key in edge_cases.keys():
		expect_equal(parsed_data[key], edge_cases[key],
			"Edge case not preserved for %s: expected '%s', got '%s'" %
			[key, edge_cases[key], str(parsed_data[key])])

func test_style_override_safety() -> void:
	"""Test that style preferences don't break string correctness"""
	var style = YAMLStyle.new()
	style.string_style = YAMLStyle.STRING_PLAIN  # Request plain style

	var test_data = {
		"empty": "",
		"boolean_like": "true",
		"numeric": "123",
		"special": "#comment",
	}

	var stringify_result = YAML.stringify(test_data, style)
	expect(not stringify_result.has_error(), stringify_result.get_error())

	if stringify_result.has_error():
		return

	var parse_result = YAML.parse(stringify_result.get_data())
	expect(not parse_result.has_error(), parse_result.get_error())

	if parse_result.has_error():
		return

	var parsed_data = parse_result.get_data()

	# Even though we requested plain style, strings should be correctly quoted
	# to prevent misinterpretation
	expect_equal(parsed_data.empty, "", "Empty string should remain empty")
	expect_equal(typeof(parsed_data.boolean_like), TYPE_STRING,
		"Boolean-like string should remain string")
	expect_equal(typeof(parsed_data.numeric), TYPE_STRING,
		"Numeric string should remain string")
	expect_equal(parsed_data.special, "#comment",
		"Special character string should be preserved")

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

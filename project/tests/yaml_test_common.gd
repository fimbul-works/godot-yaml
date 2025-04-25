extends YAMLTest
## Common YAML Test

func _init() -> void:
	test_name = "Common"

## Test errors when parsing invalid YAML
func test_parsing_errors() -> void:
	print_rich("\n[b]Testing Common Parse Errors:[/b]")

	# Test invalid YAML syntax
	var invalid_yaml := """
key: value
  indentation: wrong
"""
	assert_parse_error(invalid_yaml, "Invalid indentation detection")

	# Test unmatched quotes
	var unmatched_quotes := """
message: "This quote is not closed
next_line: value
"""
	assert_parse_error(unmatched_quotes, "Unmatched quotes detection")

	# Test invalid structure
	var invalid_structure := """
- item1
key: value
- item2
"""
	assert_parse_error(invalid_structure, "Invalid structure detection")

## Test validation of YAML content
func test_validation() -> void:
	print_rich("\n[b]Testing Common Validation:[/b]")

	# Test valid YAML
	var valid_yaml := """
key: value
list:
  - item1
  - item2
"""
	assert_validate(valid_yaml, true, "Valid YAML should pass validation")

	# Test invalid YAML
	var invalid_yaml := """
key: value
- invalid
  list
  format
"""
	assert_validate(invalid_yaml, false, "Invalid YAML should fail validation")

## Helper to truncate long text
func truncate(str: String, len := 100) -> String:
	if str.length() > len:
		return str.substr(0, len) + "..."
	return str

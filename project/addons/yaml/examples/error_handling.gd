extends Node2D
## Example showing error handling and validation with rich text printing

func _ready():
	if !visible:
		return
	print_rich("[b]⚠️ YAML Error Handling Examples[/b]")
	test_parse_errors()
	test_stringify_errors()
	test_validation()
	test_error_details()

func test_parse_errors():
	print_rich("\n[b]Testing Parse Errors:[/b]")

	# Test invalid YAML syntax
	var invalid_yaml := """
key: value
  indentation: wrong
"""
	var result := YAML.parse(invalid_yaml)
	assert(result.has_error(), "Expected error for invalid YAML")
	print_rich("[color=red]Parse Error (invalid syntax):[/color]")
	print_rich("  [i]Message:[/i] %s" % result.get_error_message())
	print_rich("  [i]Location:[/i] line %d, column %d" % [result.get_error_line(), result.get_error_column()])

	# Test unmatched quotes
	var unmatched_quotes := """
message: "This quote is not closed
next_line: value
"""
	result = YAML.parse(unmatched_quotes)
	assert(result.has_error(), "Expected error for unmatched quotes")
	print_rich("\n[color=red]Parse Error (unmatched quotes):[/color]")
	print_rich("  [i]Message:[/i] %s" % result.get_error_message())
	print_rich("  [i]Location:[/i] line %d, column %d" % [result.get_error_line(), result.get_error_column()])

func test_stringify_errors():
	print_rich("\n[b]Testing Stringify Errors:[/b]")

	# Test circular reference
	var dict1 = {}
	var dict2 = {"ref": dict1}
	dict1["circular"] = dict2

	var result := YAML.stringify(dict1)
	assert(result.has_error(), "Expected error for circular reference")
	print_rich("[color=red]Stringify Error (circular reference):[/color]")
	print_rich("  [i]Message:[/i] %s" % result.get_error_message())
	print_rich("  [i]Location:[/i] line %d, column %d" % [result.get_error_line(), result.get_error_column()])

func test_validation():
	print_rich("\n[b]Testing Validation:[/b]")

	# Test valid YAML
	var valid_yaml := """
key: value
list:
  - item1
  - item2
"""
	var result := YAML.validate(valid_yaml)
	assert(!result.has_error(), "Unexpected error for valid YAML")
	print_rich("[color=green]✓ Valid YAML passed validation[/color]")

	# Test invalid YAML
	var invalid_yaml := """
key: value
- invalid
  list
  format
"""
	result = YAML.validate(invalid_yaml)
	assert(result.has_error(), "Expected validation error")
	print_rich("\n[color=red]Validation Error:[/color]")
	print_rich("  [i]Message:[/i] %s" % result.get_error_message())
	print_rich("  [i]Location:[/i] line %d, column %d" % [result.get_error_line(), result.get_error_column()])

func test_error_details():
	print_rich("\n[b]Testing Error Details:[/b]")

	var yaml_with_error := """
valid_line: value
- invalid line: value
another_line: value
"""

	var result := YAML.parse(yaml_with_error)
	assert(result.has_error(), "Expected error with location info")
	print_rich("\n[color=red]Error Details:[/color]")
	print_rich("  [i]Message:[/i] %s" % result.get_error_message())
	print_rich("  [i]Location:[/i] line %d, column %d" % [result.get_error_line(), result.get_error_column()])

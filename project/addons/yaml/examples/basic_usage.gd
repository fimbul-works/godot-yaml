extends Node2D
## Example showing basic usage with rich text printing

var yaml_text := """
string: string_value
number: 1234
list:
  - apples
  - oranges
"""

func _ready():
	if !visible:
		return
	print_rich("[b]🔰 Basic YAML Usage Example[/b]")

	# Parse YAML string
	var parse_result := YAML.parse(yaml_text)
	assert(!parse_result.get_error_message(), parse_result.get_error_message())

	var data := parse_result.get_data()
	print_rich("\n[b]Parse Result:[/b]\n%s" % data)

	# Stringify data
	var stringify_result := YAML.stringify(data)
	assert(!stringify_result.get_error_message(), stringify_result.get_error_message())

	var yaml := stringify_result.get_data()
	print_rich("\n[b]Stringify Result:[/b]\n%s" % yaml)

	# Validate YAML
	var validate_result := YAML.validate(yaml)
	assert(!validate_result.get_error_message(), validate_result.get_error_message())

	# Show version information
	print_rich("[b]YAML Version:[/b]\n%s" % YAML.version())

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
	assert(!parse_result.has_error(), parse_result.get_error_message())

	var data := parse_result.get_data()
	print_rich("\n[b]Parse Result:[/b]\n%s" % data)

	# Stringify data
	var stringify_result := YAML.stringify(data)
	assert(!stringify_result.has_error(), stringify_result.get_error_message())

	var yaml := stringify_result.get_data()
	print_rich("\n[b]Stringify Result:[/b]\n%s" % yaml)

	# Validate YAML
	var validate_result := YAML.validate(yaml)
	assert(!validate_result.has_error(), validate_result.get_error_message())

	# Load YAML file
	var load_result := YAML.load_file("res://addons/yaml/data/simple.yaml")
	assert(!load_result.has_error(), stringify_result.get_error_message())

	var loaded_data := load_result.get_data()
	print_rich("\n[b]Loaded data:[/b]\n%s" % loaded_data)

	# Save YAML file
	var save_result := YAML.save_file(loaded_data, "user://simple.yaml")
	assert(!load_result.has_error(), stringify_result.get_error_message())

extends Node2D
## Example showing custom class serialization/deserialization

func _ready() -> void:
	if !visible:
		return
	print_rich("[b]🧪 Custom class serialization[/b]")

	# Register the class constructor
	YAML.register_class(MyCustomClass)

	# You can also pass alternative method names
	#YAML.register_class(MyCustomClass, "serialize", "deserialize")

	var custom = MyCustomClass.new("hello world", 123, PI)

	# Stringify data
	var str_result := YAML.stringify(custom)
	if str_result.has_error():
		push_error(str_result.get_error_message())
		return

	var yaml: String = str_result.get_data()
	print_rich("\n[b]Stringify Result:[/b]\n%s" % yaml)

	# Parse YAML string
	var parse_result := YAML.parse(yaml)
	if parse_result.has_error():
		push_error(parse_result.get_error_message())
		return
	var parsed_custom: MyCustomClass = parse_result.get_data()

	print_rich("\n[b]Parse Result:[/b]\n%s" % parsed_custom)
	parsed_custom.hello()

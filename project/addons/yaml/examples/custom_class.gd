extends Node2D
## Example showing custom class serialization/deserialization

func _ready() -> void:
	if !visible:
		return
	print_rich("[b]🧪 Custom class serialization[/b]")

	# Register the class constructor
	YAML.register_class(MyCustomClass)
	YAML.register_class(MyCustomResource)

	# You can also pass alternative method names
	#YAML.register_class(MyCustomClass, "serialize", "deserialize")

	# Custom (Node) class
	var object = MyCustomClass.new("hello world", 123, PI)

	# Stringify data
	var obj_str_result := YAML.stringify(object)
	if obj_str_result.has_error():
		push_error(obj_str_result.get_error_message())
		return

	var obj_yaml: String = obj_str_result.get_data()
	print_rich("\n[b]MyCustomClass Stringify Result:[/b]\n%s" % obj_yaml)

	# Parse YAML string
	var obj_parse_result := YAML.parse(obj_yaml)
	if obj_parse_result.has_error():
		push_error(obj_parse_result.get_error_message())
		return
	var parsed_obj: MyCustomClass = obj_parse_result.get_data()

	print_rich("\n[b]MyCustomClass Parse Result:[/b]\n%s" % parsed_obj)
	parsed_obj.hello()

	# Custom (Resource) class
	var resource = MyCustomResource.new("I am resource", 42, 69.69)
	var res_str_result := YAML.stringify(resource)
	if res_str_result.has_error():
		push_error(res_str_result.get_error_message())
		return

	var res_yaml: String = res_str_result.get_data()
	print_rich("\n[b]MyCustomResource Stringify Result:[/b]\n%s" % res_yaml)

	# Parse YAML string
	var res_parse_result := YAML.parse(res_yaml)
	if res_parse_result.has_error():
		push_error(res_parse_result.get_error_message())
		return
	var parsed_res: MyCustomResource = res_parse_result.get_data()

	print_rich("\n[b]MyCustomResource Parse Result:[/b]\n%s" % parsed_res)
	parsed_res.hello()

	# Try parsing an invalid resource
	var invalid_yaml := """
!MyCustomClass
this: will error
"""
	var invalid_parse_result := YAML.parse(invalid_yaml)
	assert(invalid_parse_result.has_error(), "YAML should have an error")
	print_rich("Received expected error: ", invalid_parse_result.get_error_message())

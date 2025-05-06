extends BaseTest

func _init() -> void:
	icon = "🧩"

func _enter_tree() -> void:
	YAML.register_class(MyCustomClass)
	YAML.register_class(MyCustomResource)
	YAML.register_class(MyStringClass)

func _exit_tree() -> void:
	YAML.unregister_class(MyCustomClass)
	YAML.unregister_class(MyCustomResource)
	YAML.unregister_class(MyStringClass)

func test_custom_node_class() -> void:
	var object = MyCustomClass.new("hello world", 123, PI)

	# Stringify to YAML
	var str_result := YAML.stringify(object)
	expect(!str_result.has_error(), str_result.get_error_message())

	var yaml_text: String = str_result.get_data()
	if LOG_VERBOSE:
		print_rich("\n[b]MyCustomClass Stringify Result:[/b]\n%s" % yaml_text)

	# Parse YAML string
	var parse_result := YAML.parse(yaml_text)
	expect(!parse_result.has_error(), parse_result.get_error_message())

	var obj: MyCustomClass = parse_result.get_data()

	if LOG_VERBOSE:
		print_rich("\n[b]MyCustomClass Parse Result:[/b]\n%s" % obj)
		obj.hello()

func test_custom_class_errors() -> void:
	var yaml_text = """
!MyCustomClass
string_val: foo
"""
	var result := YAML.parse(yaml_text)
	expect_equal(result.get_error_message(), "Missing int_val field", "MyCustomClass requires string_val, int_val, float_val, color_val")

	var invalid_yaml_text = """
!MyCustomClass
[1, 2, 3]
"""
	var bad_result := YAML.parse(invalid_yaml_text)
	expect_equal(bad_result.get_error_message(), "Deserializing MyCustomClass expects Dictionary, received Array")

func test_custom_string_class() -> void:
	var object = MyStringClass.new("hello world")

	var str_result := YAML.stringify(object)
	expect(!str_result.has_error(), str_result.get_error_message())

	var text: String = str_result.get_data()
	if LOG_VERBOSE:
		print_rich("\n[b]MyStringClass Stringify Result:[/b]\n%s" % text)

	# When stringifying String-based classes, a newline is added to the end
	expect_equal(text, "!MyStringClass hello world\n")

	var parse_result := YAML.parse(text)
	expect(!parse_result.has_error(), parse_result.get_error_message())

func test_custom_resource() -> void:	# Custom (Resource) class
	var resource = MyCustomResource.new("I am resource", 42, 69.69)

	var str_result := YAML.stringify(resource)
	expect(!str_result.has_error(), str_result.get_error_message())

	var yaml_text: String = str_result.get_data()

	if LOG_VERBOSE:
		print_rich("\n[b]MyCustomResource Stringify Result:[/b]\n%s" % yaml_text)

	# Parse YAML string
	var parse_result := YAML.parse(yaml_text)
	expect(!parse_result.has_error(), parse_result.get_error_message())

func test_custom_resource_errors() -> void:
	var yaml_text = """
!MyCustomResource
color_val: black
"""
	var result := YAML.parse(yaml_text)
	expect_equal(result.get_error_message(), "Missing string_val field", "MyCustomResource requires string_val, int_val, float_val, color_val")

	var invalid_yaml_text = """
!MyCustomResource
[1, 2, 3]
"""
	var bad_result := YAML.parse(invalid_yaml_text)
	expect_equal(bad_result.get_error_message(), "Deserializing MyCustomResource expects Dictionary, received Array")

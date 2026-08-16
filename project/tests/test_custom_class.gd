extends TestSuite

func _init() -> void:
	icon = "🧩"

func _enter_tree() -> void:
	YAML.register_class(YAMLExampleCustomClass, "serialize", "deserialize", "ruby:object/YAMLExampleCustomClass")
	YAML.register_class(YAMLExampleCustomResource)
	YAML.register_class(YAMLExampleStringClass)

func _exit_tree() -> void:
	YAML.unregister_class(YAMLExampleCustomClass)
	YAML.unregister_class(YAMLExampleCustomResource)
	YAML.unregister_class(YAMLExampleStringClass)

func test_has_registered_class() -> void:
	expect(!YAML.has_registered_class("NonExistentClass"), "Should return false for unregistered class")
	expect(YAML.has_registered_class("YAMLExampleCustomClass"), "Should return true for registered class")

func test_custom_node_class() -> void:
	var object = YAMLExampleCustomClass.new("hello world", 123, PI)

	# Stringify to YAML
	var str_result := YAML.stringify(object)
	expect(!str_result.has_error(), str_result.get_error())

	var yaml_text: String = str_result.get_data()
	if LOG_VERBOSE:
		print_rich("\n[b]YAMLExampleCustomClass2 Stringify Result:[/b]\n%s" % yaml_text)

	# Parse YAML string
	var parse_result := YAML.parse(yaml_text)
	expect(!parse_result.has_error(), parse_result.get_error())

	var obj: YAMLExampleCustomClass = parse_result.get_data()

	if LOG_VERBOSE:
		print_rich("\n[b]YAMLExampleCustomClass Parse Result:[/b]\n%s" % obj)
		obj.hello()

	if obj != null:
		obj.free()
	object.free()

func test_custom_tag() -> void:
	var yaml_text = """
!ruby:object/YAMLExampleCustomClass
string_val: alternate tag
int_val: 42
float_val: 1.61803398875
color_val: !Color 000000
"""
	# Parse YAML string
	var parse_result := YAML.parse(yaml_text)
	expect(!parse_result.has_error(), parse_result.get_error())

	var obj: YAMLExampleCustomClass = parse_result.get_data()

	if LOG_VERBOSE:
		print_rich("\n[b]ruby:object/YAMLExampleCustomClass Parse Result:[/b]\n%s" % obj)
		obj.hello()

	if obj != null:
		obj.free()

func test_custom_class_errors() -> void:
	var yaml_text = """
!YAMLExampleCustomClass
string_val: foo
"""
	var result := YAML.parse(yaml_text)
	expect_equal(result.get_error_message(), "Missing int_val field", "YAMLExampleCustomClass requires string_val, int_val, float_val, color_val")

	var invalid_yaml_text = """
!YAMLExampleCustomClass
[1, 2, 3]
"""
	var bad_result := YAML.parse(invalid_yaml_text)
	expect_equal(bad_result.get_error_message(), "Deserializing YAMLExampleCustomClass expects Dictionary, received Array")

func test_custom_string_class() -> void:
	var object = YAMLExampleStringClass.new("hello world")

	var str_result := YAML.stringify(object)
	expect(!str_result.has_error(), str_result.get_error())

	var text: String = str_result.get_data()
	if LOG_VERBOSE:
		print_rich("\n[b]YAMLExampleStringClass Stringify Result:[/b]\n%s" % text)

	# When stringifying String-based classes, a newline is added to the end
	expect_equal(text, "!YAMLExampleStringClass hello world\n")

	var parse_result := YAML.parse(text)
	expect(!parse_result.has_error(), parse_result.get_error())
	
	var parsed_obj = parse_result.get_data()
	if parsed_obj != null:
		parsed_obj.free()
	object.free()

func test_custom_resource() -> void: # Custom (Resource) class
	var resource = YAMLExampleCustomResource.new("I am resource", 42, 69.69)

	var str_result := YAML.stringify(resource)
	expect(!str_result.has_error(), str_result.get_error())

	var yaml_text: String = str_result.get_data()

	if LOG_VERBOSE:
		print_rich("\n[b]YAMLExampleCustomResource Stringify Result:[/b]\n%s" % yaml_text)

	# Parse YAML string
	var parse_result := YAML.parse(yaml_text)
	expect(!parse_result.has_error(), parse_result.get_error())

func test_custom_resource_errors() -> void:
	var yaml_text = """
!YAMLExampleCustomResource
color_val: black
"""
	var result := YAML.parse(yaml_text)
	expect_equal(result.get_error_message(), "Missing string_val field", "YAMLExampleCustomResource requires string_val, int_val, float_val, color_val")

	var invalid_yaml_text = """
!YAMLExampleCustomResource
[1, 2, 3]
"""
	var bad_result := YAML.parse(invalid_yaml_text)
	expect_equal(bad_result.get_error_message(), "Deserializing YAMLExampleCustomResource expects Dictionary, received Array")

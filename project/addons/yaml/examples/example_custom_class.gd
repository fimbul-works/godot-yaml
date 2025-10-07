extends ExampleBase

func _init() -> void:
	icon = "🧩"
	LOG_VERBOSE = true

func _enter_tree() -> void:
	# Register our custom classes when the node enters the tree
	YAML.register_class(MyCustomClass, "serialize", "deserialize", "ruby/object:MyCustomClass")
	YAML.register_class(MyCustomResource)
	YAML.register_class(MyStringClass)

func _exit_tree() -> void:
	# Clean up registrations when the node exits the tree
	YAML.unregister_class(MyCustomClass)
	YAML.unregister_class(MyCustomResource)
	YAML.unregister_class(MyStringClass)

func run_examples() -> void:
	run_example("Custom Node Class", custom_node_class)
	run_example("Custom Class Errors", custom_class_errors)
	run_example("String-Based Custom Class", custom_string_class)
	run_example("Custom Resource Class", custom_resource)
	run_example("Custom Resource Errors", custom_resource_errors)
	run_example("Class Registration Management", class_registration_management)

func custom_node_class() -> void:
	log_info("Creating instance of MyCustomClass...")
	var object = MyCustomClass.new("hello world", 123, PI)

	log_info("Stringifying custom class instance to YAML...")
	var str_result := YAML.stringify(object)

	if str_result.has_error():
		log_error("Stringify failed: " + str_result.get_error())
		return

	var yaml_text: String = str_result.get_data()
	log_success("Custom class stringified successfully")

	if LOG_VERBOSE:
		log_info("MyCustomClass as YAML:\n" + yaml_text)

	log_info("Parsing YAML back into MyCustomClass...")
	var parse_result := YAML.parse(yaml_text)

	if parse_result.has_error():
		log_error("Parse failed: " + parse_result.get_error())
		return

	var obj: MyCustomClass = parse_result.get_data()

	if obj is MyCustomClass:
		log_success("YAML parsed back into MyCustomClass")
		log_info("string_val: " + obj.string_val)
		log_info("int_val: " + str(obj.int_val))
		log_info("float_val: " + str(obj.float_val))
	else:
		log_error("Failed to parse back into MyCustomClass")

func custom_class_errors() -> void:
	log_info("Testing error handling with invalid MyCustomClass YAML...")

	# Missing required field
	var yaml_text = """
!MyCustomClass
string_val: foo
"""
	log_code_block(yaml_text)
	log_info("Attempting to parse with missing required fields...")

	var result := YAML.parse(yaml_text)

	if result.has_error():
		log_success("Correctly detected missing field error")
		log_info("Error message: " + result.get_error())
	else:
		log_error("Failed to detect missing required field")

	# Invalid data structure
	var invalid_yaml_text = """
!MyCustomClass
[1, 2, 3]
"""
	log_code_block(invalid_yaml_text)
	log_info("Attempting to parse with wrong data structure...")

	var bad_result := YAML.parse(invalid_yaml_text)

	if bad_result.has_error():
		log_success("Correctly detected wrong data structure error")
		log_info("Error message: " + bad_result.get_error())
	else:
		log_error("Failed to detect wrong data structure")

func custom_string_class() -> void:
	log_info("Creating instance of string-based MyStringClass...")
	var object = MyStringClass.new("hello world")

	log_info("Stringifying string-based class to YAML...")
	var str_result := YAML.stringify(object)

	if str_result.has_error():
		log_error("Stringify failed: " + str_result.get_error())
		return

	var text: String = str_result.get_data()
	log_success("String-based class stringified successfully")

	if LOG_VERBOSE:
		log_result("MyStringClass as YAML:\n" + text)

	log_info("Parsing YAML back into MyStringClass...")
	var parse_result := YAML.parse(text)

	if parse_result.has_error():
		log_error("Parse failed: " + parse_result.get_error())
		return

	var obj = parse_result.get_data()

	if obj is MyStringClass:
		log_success("YAML parsed back into MyStringClass")
		log_info("Value: " + obj.value)
	else:
		log_error("Failed to parse back into MyStringClass")

func custom_resource() -> void:
	log_info("Creating instance of MyCustomResource...")
	var resource = MyCustomResource.new("I am resource", 42, 69.69)

	log_info("Stringifying resource class to YAML...")
	var str_result := YAML.stringify(resource)

	if str_result.has_error():
		log_error("Stringify failed: " + str_result.get_error())
		return

	var yaml_text: String = str_result.get_data()
	log_success("Resource class stringified successfully")

	if LOG_VERBOSE:
		log_result("MyCustomResource as YAML:\n" + yaml_text)

	log_info("Parsing YAML back into MyCustomResource...")
	var parse_result := YAML.parse(yaml_text)

	if parse_result.has_error():
		log_error("Parse failed: " + parse_result.get_error())
		return

	var obj = parse_result.get_data()

	if obj is MyCustomResource:
		log_success("YAML parsed back into MyCustomResource")
		log_info("string_val: " + obj.string_val)
		log_info("int_val: " + str(obj.int_val))
		log_info("float_val: " + str(obj.float_val))
	else:
		log_error("Failed to parse back into MyCustomResource")

func custom_resource_errors() -> void:
	log_info("Testing error handling with invalid MyCustomResource YAML...")

	# Missing required field
	var yaml_text = """
!MyCustomResource
color_val: black
"""
	log_code_block(yaml_text)
	log_info("Attempting to parse with missing required fields...")

	var result := YAML.parse(yaml_text)

	if result.has_error():
		log_success("Correctly detected missing field error")
		log_info("Error message: " + result.get_error())
	else:
		log_error("Failed to detect missing required field")

	# Invalid data structure
	var invalid_yaml_text = """
!MyCustomResource
[1, 2, 3]
"""
	log_code_block(invalid_yaml_text)
	log_info("Attempting to parse with wrong data structure...")

	var bad_result := YAML.parse(invalid_yaml_text)

	if bad_result.has_error():
		log_success("Correctly detected wrong data structure error")
		log_info("Error message: " + bad_result.get_error())
	else:
		log_error("Failed to detect wrong data structure")

func class_registration_management() -> void:
	log_subheader("Class Registration Management")

	log_info("Checking if classes are registered...")

	if YAML.has_registered_class("MyCustomClass"):
		log_success("MyCustomClass is registered")
	else:
		log_error("MyCustomClass is not registered properly")

	if YAML.has_registered_class("MyCustomResource"):
		log_success("MyCustomResource is registered")
	else:
		log_error("MyCustomResource is not registered properly")

	if YAML.has_registered_class("MyStringClass"):
		log_success("MyStringClass is registered")
	else:
		log_error("MyStringClass is not registered properly")

	log_info("\nExample of registering with custom methods:")
	var code = """
# Register with custom method names
YAML.register_class(MyCustomClass, "to_yaml", "from_yaml")

# Methods in the class would then be:
func to_yaml():
	# Custom serialization code
	return {...}

static func from_yaml(data):
	# Custom deserialization code
	return MyCustomClass.new(...)
"""
	log_code_block(code)

	log_info("\nBest practices for class registration:")
	log_info("1. Register classes at startup in an autoload/singleton")
	log_info("2. Use consistent naming for serialize/deserialize methods")
	log_info("3. Implement thorough validation in deserialize methods")
	log_info("4. For scripts that might be unloaded, register in _enter_tree and unregister in _exit_tree")

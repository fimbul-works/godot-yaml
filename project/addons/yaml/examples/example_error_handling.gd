extends ExampleBase

func _init() -> void:
	icon = "❌"

func run_examples() -> void:
	run_example("Invalid Indentation", invalid_indentation)
	run_example("Unmatched Quotes", unmatched_quotes)
	run_example("Circular Reference", circular_reference)
	run_example("Validation Example", validation_example)
	run_example("Error Details", error_details)
	run_example("Error Handling Patterns", error_handling_patterns)

func invalid_indentation() -> void:
	var invalid_yaml := """
key: value
  indentation: wrong
"""
	log_code_block(invalid_yaml)
	log_info("Parsing YAML with invalid indentation...")

	var result := YAML.parse(invalid_yaml)

	if result.has_error():
		log_success("Correctly detected error: " + result.get_error())
		log_info("Line: " + str(result.get_error_line()))
		log_info("Column: " + str(result.get_error_column()))
		return

	log_error("Failed to detect invalid indentation")

func unmatched_quotes() -> void:
	var unmatched_quotes := """
message: "This quote is not closed
next_line: value
"""
	log_code_block(unmatched_quotes)
	log_info("Parsing YAML with unmatched quotes...")

	var result = YAML.parse(unmatched_quotes)

	if result.has_error():
		log_success("Correctly detected error: " + result.get_error())
		return

	log_error("Failed to detect unmatched quotes")

func circular_reference() -> void:
	log_info("Creating circular reference in data structure...")

	# Create a circular reference
	var dict1 = {}
	var dict2 = {"ref": dict1}
	dict1["circular"] = dict2

	log_info("Attempting to stringify circular reference...")
	var result := YAML.stringify(dict1)

	if result.has_error():
		log_success("Correctly detected error: " + result.get_error())
		return

	log_error("Failed to detect circular reference")

func validation_example() -> void:
	var invalid_yaml := """
key: value
- invalid
  list
  format
"""
	log_code_block(invalid_yaml)
	log_info("Validating incorrect YAML...")

	var result = YAML.validate_syntax(invalid_yaml)

	if result.has_error():
		log_success("Validation correctly detected error: " + result.get_error())
		return

	log_error("Validation failed to detect invalid YAML")

func error_details() -> void:
	var yaml_with_error := """
valid_line: value
- invalid line: value
another_line: value
"""
	log_code_block(yaml_with_error)
	log_info("Examining error details...")

	var result := YAML.parse(yaml_with_error)

	if result.has_error():
		log_success("Error detected: " + result.get_error())

		# Show detailed error information
		log_info("Error message: " + result.get_error_message())
		log_info("Error line: " + str(result.get_error_line()))
		log_info("Error column: " + str(result.get_error_column()))

		# Highlight the error line
		var yaml_lines = yaml_with_error.split("\n")
		if result.get_error_line() > 0 and result.get_error_line() <= yaml_lines.size():
			var error_line = yaml_lines[result.get_error_line() - 1]
			log_info("Line content: " + error_line)

			if result.get_error_column() > 0:
				var pointer = " ".repeat(result.get_error_column() - 1) + "^"
				log_info(pointer + " Error position")
		return

	log_error("Failed to detect error")

func error_handling_patterns() -> void:
	log_subheader("Error Handling Patterns")

	# Pattern 1: Try-parse pattern
	log_info("Pattern 1: Using try_parse for simplified error handling")
	var yaml_text = """
name: Example
valid: true
"""
	var data = YAML.try_parse(yaml_text)
	if data:
		log_success("try_parse succeeded")
		log_info("Data: " + str(data))
	else:
		log_error("try_parse failed")

	# Pattern 2: get_error vs get_error_message
	log_info("\nPattern 2: Detailed vs Simple Error Messages")
	var invalid_yaml = "key: [invalid"
	var result = YAML.parse(invalid_yaml)
	if result.has_error():
		log_info("Detailed error: " + result.get_error())
		log_info("Simple message: " + result.get_error_message())

	# Pattern 3: Creating custom errors
	log_info("\nPattern 3: Creating custom validation errors")

	var config_yaml = """
name: MyApp
# version is missing
"""
	var parsed = YAML.parse(config_yaml)
	if !parsed.has_error():
		var validation = validate_config(parsed.get_data())
		if validation.has_error():
			log_success("Custom validation error: " + validation.get_error_message())
		else:
			log_info("Config is valid")
	else:
		log_error("Parse error: " + parsed.get_error())

func validate_config(data):
	if !data.has("name"):
		return YAMLResult.error("Configuration missing 'name' field")
	if !data.has("version"):
		return YAMLResult.error("Configuration missing 'version' field")
	return data

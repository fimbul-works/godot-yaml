extends ExampleBase

const YAML_FILE = "res://addons/yaml/examples/data/supported_syntax.yaml"
const OUTPUT_FILE = "user://supported_syntax_copy.yaml"

var yaml_text := """
string: string_value
number: 1234
list:
  - apples
  - oranges
"""

var data

func _init() -> void:
	icon = "📝"

func run_examples() -> void:
	run_example("Validate YAML String Syntax", validate_yaml_string_syntax)
	run_example("Parse YAML Text", parse_yaml_text)
	run_example("Stringify Data", stringify_data)
	run_example("Validate File Syntax", validate_file_syntax)
	run_example("Load File", load_file)
	run_example("Save File", save_file)
	run_example("Load Saved File", load_saved_file)

func validate_yaml_string_syntax() -> void:
	log_code_block(yaml_text)

	log_info("Validating YAML string...")
	var result := YAML.validate_syntax(yaml_text)

	if result.has_error():
		log_error("Validation failed: " + result.get_error())
		return

	log_success("YAML is valid!")

func parse_yaml_text() -> void:
	log_info("Parsing YAML text...")
	var result := YAML.parse(yaml_text)

	if result.has_error():
		log_error("Parse failed: " + result.get_error())
		return

	data = result.get_data()
	log_success("YAML parsed successfully")

	if LOG_VERBOSE:
		log_result(str(data))

		log_info("Accessing data values:")
		log_info("• string value: " + data.string)
		log_info("• number value: " + str(data.number))
		log_info("• list items: " + str(data.list))

func stringify_data() -> void:
	# First ensure we have data to stringify
	if data == null:
		data = YAML.parse(yaml_text).get_data()

	log_info("Converting data structure to YAML string...")
	var result := YAML.stringify(data)

	if result.has_error():
		log_error("Stringify failed: " + result.get_error())
		return

	log_success("Data converted to YAML successfully")

	if LOG_VERBOSE:
		log_result(result.get_data())

		# Verify the round trip
		var yaml_output = result.get_data().strip_edges()
		var original = yaml_text.strip_edges()
		if yaml_output == original:
			log_success("Round-trip verification: Output matches original")
		else:
			log_warning("Round-trip produced different output (semantically equivalent)")
			log_info("Original:\n" + original)
			log_info("Output:\n" + yaml_output)

func validate_file_syntax() -> void:
	log_info("Validating YAML file: " + YAML_FILE)
	var result := YAML.validate_file_syntax(YAML_FILE)

	if result.has_error():
		log_error("File validation failed: " + result.get_error())
		return

	log_success("YAML file is valid")

func load_file() -> void:
	log_info("Loading YAML file: " + YAML_FILE)
	var result := YAML.load_file(YAML_FILE)

	if result.has_error():
		log_error("File loading failed: " + result.get_error())
		return

	data = result.get_data()
	log_success("YAML file loaded successfully")

	if LOG_VERBOSE:
		log_info("File contains " + str(data.size()) + " keys")
		log_result(str(data).substr(0, 500) + "...\n(output truncated)")

func save_file() -> void:
	# Make sure we have data
	if data == null:
		data = {"example": "data", "created": "now", "values": [1, 2, 3]}

	log_info("Saving data to YAML file: " + OUTPUT_FILE)
	var result := YAML.save_file(data, OUTPUT_FILE)

	if result.has_error():
		log_error("File saving failed: " + result.get_error())
		return

	log_success("Data saved to YAML file successfully")

	if LOG_VERBOSE:
		log_result(result.get_data())

func load_saved_file() -> void:
	log_info("Loading previously saved file: " + OUTPUT_FILE)
	var result := YAML.load_file(OUTPUT_FILE)

	if result.has_error():
		log_error("File loading failed: " + result.get_error())
		return

	var loaded_data = result.get_data()
	log_success("Saved file loaded successfully")

	if LOG_VERBOSE:
		log_result(str(loaded_data))

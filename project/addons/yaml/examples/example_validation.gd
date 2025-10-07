extends ExampleBase

var schema: Schema

func _init() -> void:
	icon = "🛡️"

func run_examples() -> void:
	run_example("Load YAML Schema", load_yaml_schema)
	run_example("Validate Using Schema", validate_using_schema)
	run_example("Validate While Parsing", validate_while_parsing)
	run_example("Validate With Defaults", validate_with_defaults)

func load_yaml_schema() -> void:
	var yaml_schema_text = """
# The $id string is used as an identifier
$id: "http://example.com/user.yaml"

# Definitions
$defs:
  settings:
	type: object
	properties:
	  theme:
		type: string
		default: dark

type: object
properties:
  username:
	type: string
	minLength: 3
	maxLength: 20
  email:
	type: string
	format: email
  role:
	type: string
	default: user
	x-yaml-tag: UserRole
  settings:
	$ref: "#/$defs/settings"
required:
- username
- email
""".replace("\t", "    ")

	schema = YAML.load_schema_from_string(yaml_schema_text)
	if !schema:
		log_error("Schema failed to load!")
	else:
		log_success("Schema '%s' loaded" % schema.get_schema_definition().get("$id"))

func validate_using_schema() -> void:
	# Validate using the Schema object
	log_subheader("Successful validation")
	var result := schema.validate({
		"username": "alice",
		"email": "alice@example.com"
	})
	if result.is_valid():
		log_success(result.get_summary())
	else:
		log_error(result.get_summary())

	log_subheader("Failed validation")
	result = schema.validate({
		"username": "alice",
		"email": "invalid email"
	})
	if !result.is_valid():
		log_success(result.get_summary())
	else:
		log_error(result.get_summary())

func validate_while_parsing() -> void:
	# YAML tag validation
	var tagged_yaml = """
username: bob
email: bob@example.com
role: !UserRole admin
settings:
  theme: white
"""
	# Parse and validate using Schema $id property
	var result := YAML.parse_and_validate(tagged_yaml, "http://example.com/user.yaml")
	if result.has_validation_errors():
		log_error(result.get_validation_summary())
		return

	log_success(result.get_data())

func validate_with_defaults() -> void:
	# Default values
	var yaml_str = """
$schema: "http://example.com/user.yaml" # This refers to the $id
username: alice
email: alice@example.com
settings: {} # Empty dictionary is required to set default values
"""
	# Parse and validate using YAML $schema property
	var result := YAML.parse_and_validate(yaml_str)
	if result.has_validation_errors():
		log_error(result.get_validation_summary())
		return

	log_success(result.get_data())

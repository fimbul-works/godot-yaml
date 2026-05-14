extends TestSuite

var child_schema: Schema
var parent_schema: Schema

func _init() -> void:
	icon = "✓"
	#FILTER_PATTERNS = ["nested_custom_class"]

func _enter_tree() -> void:
	YAML.register_class(ValidationTestParent)
	YAML.register_class(ValidationTestChild)

	child_schema = Schema.build_schema({
		"$id": "http://example.com/child.json",
		"type": "object",
		"x-yaml-tag": "ValidationTestChild",
		"properties": {
			"name": {
				"type": "string",
				"minLength": 2
			},
			"age": {
				"type": "integer",
				"minimum": 0,
				"maximum": 150
			}
		},
		"required": ["name", "age"]
	}, true)

	parent_schema = Schema.build_schema({
		"$id": "http://example.com/parent.json",
		"type": "object",
		"properties": {
			"title": {
				"type": "string",
				"minLength": 3,
				"x-yaml-tag": "ValidationTestParent"
			},
			"child": { "$ref": "http://example.com/child.json" }
		},
		"required": ["title", "child"]
	}, true)

func _exit_tree() -> void:
	YAML.unregister_class(ValidationTestParent)
	YAML.unregister_class(ValidationTestChild)

func _ready() -> void:
	# Register schemas for testing
	register_test_schemas()

func register_test_schemas() -> void:
	# Player schema with defaults and tags
	var player_schema = Schema.build_schema({
		"$id": "http://example.com/player.json",
		"type": "object",
		"properties": {
			"name": {"type": "string", "minLength": 1},
			"health": {"type": "integer", "minimum": 0, "maximum": 100, "default": 100},
			"level": {"type": "integer", "minimum": 1, "default": 1},
			"class": {"type": "string", "x-yaml-tag": "PlayerClass"},
			"inventory": {
				"type": "array",
				"items": {"x-yaml-tag": "Item"}
			}
		},
		"required": ["name"]
	}, true)

	# Config schema that references other files
	var config_schema = Schema.build_schema({
		"$id": "http://example.com/config.json",
		"type": "object",
		"properties": {
			"player": {"$ref": "http://example.com/player.json"},
			"settings": {
				"type": "object",
				"properties": {
					"volume": {"type": "number", "minimum": 0, "maximum": 1, "default": 0.5},
					"fullscreen": {"type": "boolean", "default": false}
				}
			}
		}
	}, true)

	# Nested config schema
	var nested_schema = Schema.build_schema({
		"$id": "http://example.com/nested.json",
		"type": "object",
		"properties": {
			"data": {"$ref": "http://example.com/player.json"}
		}
	}, true)

func test_explicit_schema_with_defaults() -> void:
	var yaml = """
name: Hero
class: !PlayerClass "Warrior"
"""

	var result = YAML.parse_and_validate(yaml, "http://example.com/player.json")

	expect(!result.has_validation_errors(), "Should validate successfully")

	var data = result.get_data()
	expect_equal(data.name, "Hero", "Name should be parsed")
	expect_equal(data.health, 100, "Health default should be applied")
	expect_equal(data.level, 1, "Level default should be applied")
	expect_equal(data["class"], "Warrior", "Class should be parsed")

func test_auto_validation_with_schema_field() -> void:
	var yaml = """
$schema: http://example.com/player.json
name: Knight
"""

	# No explicit schema - should discover from $schema field
	var result = YAML.parse_and_validate(yaml, null)

	expect(!result.has_validation_errors(), "Should auto-validate")

	var data = result.get_data()
	expect_equal(data.health, 100, "Default should be applied from discovered schema")

func test_regular_parse_ignores_schema() -> void:
	var yaml = """
$schema: http://example.com/player.json
name: Mage
health: 80
"""

	# Regular parse() ignores $schema field - no validation
	var result = YAML.parse(yaml)

	expect(!result.has_validation_result(), "parse() should not validate")
	expect_equal(result.get_data().health, 80, "Should parse without validation")

func test_validation_failure_with_errors() -> void:
	var yaml = """
$schema: http://example.com/player.json
health: 150
"""

	var result = YAML.parse_and_validate(yaml)

	expect(result.has_validation_errors(), "Should have validation errors")

	var errors = result.get_validation_errors()
	var has_required_error = false
	var has_maximum_error = false

	for error in errors:
		if error.keyword == "required":
			has_required_error = true
		if error.keyword == "maximum":
			has_maximum_error = true

	expect(has_required_error, "Should have 'required' error for missing name")
	expect(has_maximum_error, "Should have 'maximum' error for health > 100")

func test_yaml_tag_validation() -> void:
	var yaml_wrong_tag = """
$schema: http://example.com/player.json
name: Rogue
class: !WrongTag "Assassin"
"""

	var result = YAML.parse_and_validate(yaml_wrong_tag)

	expect(result.has_validation_errors(), "Should fail with wrong tag")

	var errors = result.get_validation_errors()
	var has_tag_error = false
	for error in errors:
		if error.keyword == "x-yaml-tag":
			has_tag_error = true
			expect(error.instance_path.contains("class"), "Error should be for 'class' property")

	expect(has_tag_error, "Should have x-yaml-tag constraint error")

func test_yaml_tag_validation_in_arrays() -> void:
	var yaml_wrong_item_tag = """
$schema: http://example.com/player.json
name: Warrior
inventory:
  - !Item "Sword"
  - !WrongTag "Shield"
"""

	var result = YAML.parse_and_validate(yaml_wrong_item_tag)

	expect(result.has_validation_errors(), "Should fail with wrong item tag")

	var errors = result.get_validation_errors()
	var has_item_tag_error = false
	for error in errors:
		if error.keyword == "x-yaml-tag" and error.instance_path.contains("inventory"):
			has_item_tag_error = true

	expect(has_item_tag_error, "Should have x-yaml-tag error for array item")

func test_nested_resource_validation() -> void:
	# Create test YAML files
	var player_yaml = """
$schema: http://example.com/player.json
name: ResourceHero
class: !PlayerClass "Paladin"
"""

	var file = FileAccess.open("user://test_player.yaml", FileAccess.WRITE)
	file.store_string(player_yaml)
	file.close()

	var config_yaml = """
$schema: http://example.com/config.json
player: !Resource "user://test_player.yaml"
"""

	var result = YAML.parse_and_validate(config_yaml)

	expect(!result.has_validation_errors(), "Nested resource should validate")

	var data = result.get_data()
	expect_equal(data.player.name, "ResourceHero", "Nested resource should be loaded")
	expect_equal(data.player.health, 100, "Nested resource defaults should apply")

func test_nested_resource_error_paths() -> void:
	# Create invalid nested YAML
	var bad_player_yaml = """
$schema: http://example.com/player.json
health: 150
"""

	var file = FileAccess.open("user://bad_player.yaml", FileAccess.WRITE)
	file.store_string(bad_player_yaml)
	file.close()

	var config_yaml = """
$schema: http://example.com/config.json
player: !Resource "user://bad_player.yaml"
"""

	var result = YAML.parse_and_validate(config_yaml)

	expect(result.has_validation_errors(), "Should have errors from nested resource")

	var errors = result.get_validation_errors()
	var found_resource_error = false

	for error in errors:
		var path = error.instance_path
		if path.contains("!Resource(user://bad_player.yaml)"):
			found_resource_error = true

	expect(found_resource_error, "Should have error with !Resource() in path")

func test_nested_defaults() -> void:
	var yaml = """
$schema: http://example.com/config.json
player:
  name: DefaultTester
settings: {}
"""

	var result = YAML.parse_and_validate(yaml)

	expect(!result.has_validation_errors(), "Should validate")

	var data = result.get_data()
	expect_equal(data.player.health, 100, "Nested player defaults should apply")
	expect_equal(data.settings.volume, 0.5, "Settings defaults should apply")
	expect_equal(data.settings.fullscreen, false, "Nested settings defaults should apply")

func test_parse_and_validate_with_null_discovers_schema() -> void:
	var yaml = """
$schema: http://example.com/player.json
name: NullParamTest
"""

	# Explicitly pass null - should still discover
	var result = YAML.parse_and_validate(yaml, null)

	expect(!result.has_validation_errors(), "Should discover schema even with explicit null")
	expect_equal(result.get_data().health, 100, "Defaults should apply")

func test_no_validation_without_schema() -> void:
	var yaml = """
name: NoSchema
health: 999
arbitrary: "whatever"
"""

	var result = YAML.parse_and_validate(yaml)

	expect(!result.has_validation_result(), "Should not have validation result")
	expect_equal(result.get_data().health, 999, "Invalid data should parse without schema")

func test_validation_error_summary() -> void:
	var yaml = """
$schema: http://example.com/player.json
health: -50
level: 0
"""

	var result = YAML.parse_and_validate(yaml)

	expect(result.has_validation_errors(), "Should have multiple errors")

	var summary = result.get_validation_summary()
	expect(summary.length() > 0, "Summary should not be empty")
	expect(summary.contains("error"), "Summary should mention errors")

func test_load_from_yaml_string() -> void:
	# Test valid YAML schema
	var yaml_string = """
type: object
properties:
  username:
	type: string
	minLength: 3
	maxLength: 20
  age:
	type: integer
	minimum: 0
required:
- username
""".replace("\t", "    ")

	var schema = YAML.load_schema_from_string(yaml_string)

	expect(schema != null, "Schema should load from valid YAML string")
	expect(schema.is_valid(), "Loaded schema should compile successfully")

	# Test that the loaded schema works correctly
	var valid_data = {"username": "alice", "age": 25}
	var result = schema.validate(valid_data)
	expect(result.is_valid(), "Valid data should pass validation")

func test_load_from_yaml_invalid() -> void:
	# Test with malformed YAML
	var bad_yaml = """
type: string
  indentation: wrong
"""
	var schema = YAML.load_schema_from_string(bad_yaml)

	expect(schema == null, "Malformed YAML should return null")

	# Test with YAML that's not an object
	var array_yaml = """
- array
- is
- invalid
"""
	schema = YAML.load_schema_from_string(array_yaml)

	expect(schema == null, "YAML array should return null (schemas must be objects)")

func test_load_from_yaml_with_id() -> void:
	# Test that schemas with $id are auto-registered
	var yaml_with_id = """
"$id": http://example.com/test-schema-from-json
type: string
minLength: 5
"""

	var schema = YAML.load_schema_from_string(yaml_with_id)

	expect(schema != null, "Schema with $id should load")
	expect(Schema.is_schema_registered("http://example.com/test-schema-from-json"), "Schema with $id should be auto-registered")

	# Clean up
	Schema.unregister_schema("http://example.com/test-schema-from-json")

func test_load_from_yaml_with_meta_validation() -> void:
	# Test loading with meta-schema validation enabled
	var valid_schema_yaml = """
type: string
minLength: 1
x-yaml-tag: TestTag
default: "foobar"
"""
	var schema := YAML.load_schema_from_string(valid_schema_yaml, true)

	expect(schema != null, "Schema object should be created")
	expect(schema.is_valid(), "Valid schema should pass meta-validation")

	# Test with schema that fails meta-validation
	var invalid_schema_yaml = """
type: string
minLength: not a number
"""
	schema = YAML.load_schema_from_string(invalid_schema_yaml, true)

	expect(schema != null, "Invalid schema should still return Schema object")
	expect(!schema.is_valid(), "Schema with meta-validation errors should be invalid")
	expect(schema.get_compile_errors().size() > 0, "Should have compilation errors from meta-validation")

func test_nested_custom_class_validation_valid() -> void:
	var yaml = """
!ValidationTestParent
$schema: http://example.com/parent.json
title: "Parent Title"
child: !ValidationTestChild
  name: Alice
  age: 25
"""

	var result = YAML.parse_and_validate(yaml)

	expect(!result.has_validation_errors(), "Valid nested structure should pass validation")

	var parent = result.get_data()
	expect(parent is ValidationTestParent, "Should be ValidationTestParent instance")
	if parent != null:
		expect_equal(parent.title, "Parent Title", "Parent title should match")
		expect(parent.child is ValidationTestChild, "Child should be ValidationTestChild instance")
		expect_equal(parent.child.name, "Alice", "Child name should match")
		expect_equal(parent.child.age, 25, "Child age should match")

func test_nested_custom_class_validation_invalid_child() -> void:
	# Invalid: child age exceeds maximum
	var yaml_invalid_age = """
!ValidationTestParent
$schema: http://example.com/parent.json
title: "Parent Title"
child: !ValidationTestChild
  name: Bob
  age: 200
"""

	var result = YAML.parse_and_validate(yaml_invalid_age)

	expect(result.has_validation_errors(), "Should fail validation for invalid child age")

	var errors = result.get_validation_errors()
	var has_age_error = false
	for error in errors:
		if error.keyword == "maximum" and error.instance_path.contains("age"):
			has_age_error = true
			break

	expect(has_age_error, "Should have maximum validation error for child age")

func test_nested_custom_class_validation_missing_required() -> void:
	# Missing required child.name field
	var yaml = """
!ValidationTestParent
$schema: http://example.com/parent.json
title: "Parent Title"
child: !ValidationTestChild
  age: 30
"""

	var result = YAML.parse_and_validate(yaml)

	expect(result.has_validation_errors(), "Should fail validation for missing required field")

	var errors = result.get_validation_errors()
	print(result)
	print("ERRORS: ", errors)

	var has_required_error = false
	for error in errors:
		if error.keyword == "required" and error.instance_path.contains("child"):
			has_required_error = true
			break

	expect(has_required_error, "Should have required field validation error for child")

func test_reported_issue_20() -> void:
	var file := FileAccess.open("res://tests/data/schema_test.yaml", FileAccess.READ)
	var yaml_text := file.get_as_text()
	var yaml_dict  = YAML.parse(yaml_text).get_data()
	file.close()

	print("DICT: ", yaml_dict)

	var schema_str = FileAccess.get_file_as_string("res://tests/data/schemas/schema_test.json")
	var schema_dict = JSON.parse_string(schema_str)
	var schema = Schema.build_schema(schema_dict, true)

	print("SCHEMA: ", schema.get_schema_definition())

	var result  = schema.validate(yaml_dict)
	print("Schema validation:")
	print(result.get_errors())

	var parse_and_validate_result = YAML.parse_and_validate(yaml_text, schema)
	print("YAML parse_and_validate:")
	print(parse_and_validate_result)
	print("ERRORS:")
	print(parse_and_validate_result.get_validation_errors())
	pass

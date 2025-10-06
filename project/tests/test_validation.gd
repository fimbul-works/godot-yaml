extends TestSuite

func _init() -> void:
	icon = "✓"

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
	expect_equal(data.player.get("health"), 100, "Nested player defaults should apply")
	expect_equal(data.get("settings", {}).get("volume"), 0.5, "Settings defaults should apply")
	expect_equal(data.get("settings", {}).get("fullscreen"), false, "Nested settings defaults should apply")

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

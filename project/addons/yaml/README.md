# Godot YAML

A high-performance YAML parsing and serialization plugin for Godot 4.3, powered by [RapidYAML](https://github.com/biojppm/rapidyaml). This plugin offers comprehensive YAML support with customizable styling options, full Godot variant type handling, custom class serialization, and industry-standard schema validation.

**New to YAML in Godot?** Check out the [`examples/`](./addons/yaml/examples/) directory for comprehensive usage examples covering all features.

## Version History

- **2.1.1** (Current) - Empty strings are stringified with quotes
- **2.1.0** - When passing custom tag to `YAML.schema_register(class, serialize_method, deserialize_static, custom_tag)` use the custom tag when stringifying
- **2.0.0** - Major release with schema validation powered by [GDSchema](https://github.com/fimbul-works/gdschema), improved multi-document handling, and bug fixes. See [the full changelog](./CHANGELOG.md#version-200) for details
- **1.1.0** - YAML files can now be loaded with the `!Resource` tag, and some fixes
- **1.0.0** - First major release with custom YAML editor, streamlined API, and several fixes. See [the full changelog and a migration guide](./CHANGELOG.md#version-100) for details
- **0.12.1** - Build support for Linux (x86 64-bit)
- **0.12.0** - Performance optimizations, bug fixes, and comprehensive tests for all variant types
- **0.11.0** - Added support for parsing multiple documents, and error handling for custom class deserialization
- **0.10.1** - Fixed issue with custom Resources not being serializable
- **0.10.0** - Added custom class serialization support, upgraded to Godot 4.3
- **0.9.0** - Initial public release

## Features

- ⚡ **High Performance**: Built on the lightweight and efficient [RapidYAML](https://github.com/biojppm/rapidyaml) library
- 🧩 **Comprehensive Variant Support**: Handles all Godot built-in Variant types (except Callable and RID)
- ✅ **Schema Validation**: Full JSON Schema Draft-7 validation powered by [GDSchema](https://github.com/fimbul-works/gdschema) with YAML-specific extensions
- 🧪 **Custom Class Serialization**: Register your GDScript classes for seamless serialization and deserialization
- 📄 **Multi-Document Support**: Parse YAML files with multiple `---` separated documents
- 🎨 **Style Customization**: Control how YAML is formatted with customizable style options
- 📝 **Comprehensive Error Handling**: Detailed error reporting with line and column information
- 🔀 **Thread-Safe**: Fully supports multi-threaded parsing and emission
- 🗂️ **Resource References**: Use `!Resource` tags to reference and load external resources
- 🛡️ **Security Controls**: Manage resource loading security during YAML parsing

## Compatibility

- Requires **Godot 4.3** or higher
- Supported platforms:
  - Windows (x86 64-bit)
  - Linux (x86 64-bit)
  - macOS: (Universal)
    -  **Note**: Some macOS configurations (particularly newer versions with stricter Gatekeeper policies) may prevent loading of GDExtensions generally, not just this plugin. If the extension fails to load, try building from source or test with other GDExtensions to determine if this is a system-wide issue.

## Basic Usage

### Parsing YAML

```gdscript
# Parse a YAML string
var yaml_text = """
player:
  name: Knight
  health: 100
  inventory:
    - Sword
    - Shield
    - Health Potion
"""

var result = YAML.parse(yaml_text)
if result.has_error():
    push_error("Parse error: %s" % result.get_error())
    return

var data = result.get_data()
print("Player name: %s" % data.player.name)
print("Health: %d" % data.player.health)
print("First item: %s" % data.player.inventory[0])
```

### Converting Data to YAML

```gdscript
# Convert Godot data to YAML
var enemy_data = {
    "name": "Dragon",
    "health": 500,
    "attacks": ["Bite", "Fire Breath", "Tail Whip"]
}

var string_result = YAML.stringify(enemy_data)
if !string_result.has_error():
    print(string_result.get_data())
```

### Working with Files

```gdscript
# Load YAML from a file
var result = YAML.load_file("res://data/level_data.yaml")

if result.has_error():
    push_error("YAML parsing failed: " + result.get_error())
    return

# Success - get the data and use it
var level_data = result.get_data()
print("Loaded level: " + level_data.name)

# Save data to a YAML file
var save_data = {
    "player": {
        "name": "Hero",
        "level": 10,
        "position": [25, 48]
    },
    "quests_completed": ["Rats in the Cellar", "Lost Artifact"]
}

var save_result = YAML.save_file(save_data, "user://save_game.yaml")
if !save_result.has_error():
    print("Game saved successfully!")
else:
    push_error("Save failed: " + save_result.get_error())
```

### Simplified API

The extension provides simplified methods that return direct results rather than YAMLResult objects:

```gdscript
# Quick parsing without error checking
var data = YAML.try_parse("""
weapon: Axe
damage: 25
""")
# Or YAML.try_load_file

if data:
    print("Weapon: %s (Damage: %d)" % [data.weapon, data.damage])
else:
    print("Failed to parse weapon data")

# Quick stringify
var npc = {
    "name": "Merchant",
    "dialog": "Welcome to my shop!",
    "shop_items": ["Potion", "Map", "Torch"]
}

var yaml_text = YAML.try_stringify(npc)
# Or YAML.try_save_file
if yaml_text:
    save_to_file(yaml_text)
```

## Schema Validation

Version 2.0.0 introduces powerful schema validation capabilities through the integration of [GDSchema](https://github.com/fimbul-works/gdschema). Define your data structures using industry-standard JSON Schema Draft-7 syntax, enhanced with YAML-specific features for an optimal validation experience.

### Why Use Schema Validation?

Schema validation ensures your YAML data meets specific requirements before your game uses it. This is invaluable for:

- **Configuration files**: Validate game settings, difficulty parameters, and preferences
- **User-generated content**: Ensure mod data and custom levels follow your specifications
- **Save files**: Verify save data integrity with automatic default values
- **Data interchange**: Validate API responses and external data sources
- **Development**: Catch data errors early with detailed validation reports

### Quick Start

The `YAML.parse_and_validate()` method combines parsing and validation in one step, returning a `YAMLResult` that may contain both parse errors and validation errors:

```gdscript
# Define a schema in YAML (more readable than JSON!)
var schema_yaml = """
type: object
properties:
  username:
    type: string
    minLength: 3
    maxLength: 20
  email:
    type: string
    format: email
  level:
    type: integer
    minimum: 1
    default: 1
required:
- username
- email
"""

# Build the schema
var schema = YAML.load_schema_from_string(schema_yaml)
if not schema:
    push_error("Failed to parse schema")
    return

# Parse and validate YAML data
var player_yaml = """
username: hero
email: hero@example.com
"""

var result = YAML.parse_and_validate(player_yaml, schema)

# Check for parse errors first
if result.has_error():
    push_error("Parse error: %s" % result.get_error())
    return

# Then check for validation errors
if result.has_validation_errors():
    print(result.get_validation_summary())  # Detailed error report
    return

# Success - use the validated data with defaults applied
var player_data = result.get_data()
print("Level: %d" % player_data.level)  # 1 (default applied!)
```

### YAML-Specific Schema Extensions

Godot YAML includes two powerful extensions to standard JSON Schema:

#### 1. The `default` Keyword

Automatically apply default values when properties are missing:

```gdscript
var schema = YAML.load_schema_from_string("""
type: object
properties:
  difficulty:
    type: string
    enum: [easy, normal, hard]
    default: normal
  music_volume:
    type: number
    minimum: 0
    maximum: 1
    default: 0.8
  show_tutorial:
    type: boolean
    default: true
""")

# Parse empty YAML - defaults are applied during validation!
var result = YAML.parse_and_validate('{}', schema)

if !result.has_validation_errors():
    var settings = result.get_data()
    print(settings.difficulty)      # "normal"
    print(settings.music_volume)    # 0.8
    print(settings.show_tutorial)   # true
```

#### 2. The `x-yaml-tag` Keyword

Validate that values have the correct YAML type tag:

```gdscript
var schema = YAML.load_schema_from_string("""
type: object
properties:
  player_sprite:
    type: string
    x-yaml-tag: Resource      # Must be tagged with !Resource
  custom_item:
    type: object
    x-yaml-tag: Item          # Must be tagged with !Item
""")

# This YAML will validate successfully
var valid_yaml = """
player_sprite: !Resource "res://player.png"
custom_item: !Item
  name: Sword
  damage: 10
"""

var result = YAML.parse_and_validate(valid_yaml, schema)
if !result.has_validation_errors():
    var data = result.get_data()
    print("Sprite loaded: %s" % (data.player_sprite is Texture2D))
```

### Reusable Schemas with Global Registration

Schemas with an `$id` field are automatically registered globally, enabling modular schema design:

```gdscript
# Define a reusable schema (saved as user_schema.yaml)
var user_schema_yaml = """
$id: "http://mygame.com/schemas/user.yaml"
type: object
properties:
  username:
    type: string
    minLength: 3
  email:
    type: string
    format: email
  avatar:
    type: string
    x-yaml-tag: Resource
required:
- username
- email
"""

# Load and auto-register the schema
var user_schema = YAML.load_schema_from_string(user_schema_yaml)

# Now reference it from other schemas!
var game_data_schema = YAML.load_schema_from_string("""
type: object
properties:
  player:
    $ref: "http://mygame.com/schemas/user.yaml"
  high_score:
    type: integer
    minimum: 0
""")

# Parse and validate nested YAML data
var game_yaml = """
player:
  username: alice
  email: alice@example.com
high_score: 1000
"""

var result = YAML.parse_and_validate(game_yaml, game_data_schema)

if result.has_error():
    push_error("Parse error: %s" % result.get_error())
elif result.has_validation_errors():
    push_error(result.get_validation_summary())
else:
    var game_data = result.get_data()
    print("Player: %s, Score: %d" % [game_data.player.username, game_data.high_score])
```

### Parse and Validate in One Step

For the most streamlined workflow, use `YAML.parse_and_validate()` which returns a `YAMLResult` with integrated validation information:

```gdscript
# Load a schema file once at startup
var config_schema = YAML.load_schema_from_file("res://schemas/config_schema.yaml")

# Later, parse and validate user config in one call
var user_config = """
graphics:
  resolution: 1920x1080
  vsync: true
audio:
  master_volume: 0.8
"""

var result = YAML.parse_and_validate(user_config, config_schema)

# Check for parse errors
if result.has_error():
    push_error("YAML parse error: %s" % result.get_error())
    return

# Check for validation errors using YAMLResult methods
if result.has_validation_errors():
    # Show detailed error report to user
    print("Configuration has %d error(s):" % result.get_validation_error_count())
    print(result.get_validation_summary())

    # Or iterate through individual errors
    for error in result.get_validation_errors():
        print("  - %s at %s" % [error.message, error.instance_path])
    return

# Both parsing and validation succeeded
var config = result.get_data()
apply_settings(config)
```

### Auto-Discovery with `$schema`

Include a `$schema` field in your YAML to automatically validate against a registered schema:

```gdscript
# Register your schema once
YAML.load_schema_from_file("res://schemas/save_game.yaml")
# This schema has: $id: "http://mygame.com/schemas/save_game.yaml"

# YAML files can reference the schema directly
var save_data = """
$schema: "http://mygame.com/schemas/save_game.yaml"
player:
  name: Hero
  level: 10
checkpoint: forest_entrance
"""

# Parse and validate - no need to specify schema!
var result = YAML.parse_and_validate(save_data)

# YAMLResult provides validation checking methods
if result.has_error():
    push_error("Parse error: %s" % result.get_error())
elif result.has_validation_errors():
    push_error("Validation failed:\n%s" % result.get_validation_summary())
else:
    # Safe to use - both parsing and validation succeeded
    load_game(result.get_data())
```

### Advanced Schema Features

JSON Schema Draft-7 offers powerful composition and validation features:

```gdscript
var advanced_schema = YAML.load_schema_from_string("""
$defs:
  # Reusable definitions
  positive_integer:
    type: integer
    minimum: 1

  item_base:
    type: object
    properties:
      name:
        type: string
        minLength: 1
      value:
        $ref: "#/$defs/positive_integer"
    required: [name, value]

type: object
properties:
  inventory:
    type: array
    items:
      # Logical composition - item must match base AND have quantity
      allOf:
      - $ref: "#/$defs/item_base"
      - type: object
        properties:
          quantity:
            $ref: "#/$defs/positive_integer"

  equipment:
    # Conditional validation
    if:
      properties:
        type:
          const: weapon
    then:
      properties:
        damage:
          type: integer
          minimum: 1
      required: [damage]
    else:
      properties:
        defense:
          type: integer
          minimum: 1
      required: [defense]
""")
```

### Detailed Validation Error Reports

When validation fails, `YAMLResult` provides comprehensive error information through dedicated methods:

```gdscript
var schema = YAML.load_schema_from_string("""
type: object
properties:
  player:
    type: object
    properties:
      name:
        type: string
        minLength: 3
      age:
        type: integer
        minimum: 0
      email:
        type: string
        format: email
    required: [name, email]
""")

var invalid_yaml = """
player:
  name: ab
  age: -5
  email: not-email
"""

var result = YAML.parse_and_validate(invalid_yaml, schema)

# Check parse error first
if result.has_error():
    push_error("YAML syntax error: %s" % result.get_error())
    return

# Then check validation with YAMLResult methods
if result.has_validation_errors():
    # Get formatted summary for display
    print(result.get_validation_summary())
    # Schema validation failed with 3 error(s):
    #   [1] At '/player/name': String length 2 is less than minimum 3 (minLength)
    #   [2] At '/player/age': Value -5 is less than minimum 0 (minimum)
    #   [3] At '/player/email': String "not-email" does not match format email (format)

    # Get error count
    print("\nTotal errors: %d" % result.get_validation_error_count())

    # Iterate through individual errors
    for error in result.get_validation_errors():
        print("\nError details:")
        print("  Path: %s" % error.instance_path)
        print("  Constraint: %s" % error.keyword)
        print("  Message: %s" % error.message)
        print("  Invalid value: %s" % error.invalid_value)

    # Or use the full SchemaValidationResult for advanced inspection
    var validation = result.get_validation_result()
    print("\nAll error paths: %s" % validation.get_all_error_paths())
    print("Violated constraints: %s" % validation.get_violated_constraints())
```

For more information on JSON Schema features, see the [GDSchema documentation](https://github.com/fimbul-works/gdschema).

### Understanding Parse vs Validation Errors

When using `YAML.parse_and_validate()`, the `YAMLResult` may contain two types of errors:

1. **Parse Errors** (checked with `has_error()`): YAML syntax errors that prevent parsing
2. **Validation Errors** (checked with `has_validation_errors()`): Schema validation failures after successful parsing

Always check parse errors first, as validation only happens if parsing succeeds:

```gdscript
var result = YAML.parse_and_validate(yaml_text, schema)

# Check parse error first
if result.has_error():
    print("YAML syntax error at line %d: %s" % [
        result.get_error_line(),
        result.get_error_message()
    ])
    return

# Then check validation
if result.has_validation_errors():
    print("Data is valid YAML but doesn't match schema:")
    print(result.get_validation_summary())
    return

# Both checks passed - safe to use
var data = result.get_data()
```

## Multi-Document YAML Support

```gdscript
var yaml_text = """
# Player stats
name: Hero
health: 100
---
# Game settings
difficulty: hard
enable_tutorial: false
"""

var result = YAML.parse(yaml_text)
if !result.has_error():
    # Check if the result contains multiple documents
    print("Has multiple documents: %s" % result.has_multiple_documents())

    # Check document count
    var doc_count = result.get_document_count()
    print("Found %d documents" % doc_count)

    # Get the documents
    var player_data = result.get_document(0)
    var settings = result.get_document(1)

    print("Player: %s (Health: %d)" % [player_data.name, player_data.health])
    print("Difficulty: %s" % settings.difficulty)

    # Get documents as an array
    var docs = result.get_documents()
```

## Error Handling

The `YAMLResult` class provides detailed error information:

```gdscript
var result = YAML.parse(user_yaml)

if result.has_error():
    push_error("YAML parse error: " + result.get_error())
    # Example output: "parse error (line 3, column 5)"

    # Get detailed error information
    var error_message = result.get_error_message()
    var error_line = result.get_error_line()
    var error_column = result.get_error_column()

    print("Error at line %d, column %d: %s" % [error_line, error_column, error_message])

    # Highlight the error position
    if error_line > 0 and error_column > 0:
        var yaml_lines = yaml_text.split("\n")
        var error_line_content = yaml_lines[error_line - 1]
        print(error_line_content)
        print(" ".repeat(error_column - 1) + "^ Error here")
```

## Custom Class Serialization

You can register your custom GDScript classes for seamless serialization:

```gdscript
# Define a custom class
class_name Item extends Resource

var name: String
var weight: float
var value: int

func _init(p_name = "", p_weight = 0.0, p_value = 0):
    name = p_name
    weight = p_weight
    value = p_value

static func deserialize(data):
    if typeof(data) != TYPE_DICTIONARY:
        return YAMLResult.error("Item requires a dictionary")

    return Item.new(
        data.get("name", ""),
        data.get("weight", 0.0),
        data.get("value", 0)
    )

func serialize():
    return {
        "name": name,
        "weight": weight,
        "value": value
    }

# Register the class for YAML serialization
YAML.register_class(Item)

# Now we can serialize/deserialize Item objects
var sword = Item.new("Iron Sword", 5.0, 100)
var result = YAML.stringify(sword)
print(result.get_data())
# Output: !Item {name: Iron Sword, weight: 5.0, value: 100}
```

## Security Controls with YAMLSecurity

The `YAMLSecurity` class helps guard against unsafe loading of untrusted content:

```gdscript
var security = YAML.create_security()

# Only allow textures from the game's asset folder
security.allow_path("res://assets/textures", ["Texture2D"])

# Block scenes for safety
security.block_type("PackedScene")

# Parse YAML with custom security settings
var yaml_text = """
player:
  name: Hero
  sprite: !Resource 'res://assets/textures/player.png'
"""

var result = YAML.parse(yaml_text, security)
if result.has_error():
    push_error(result.get_error())
else:
    var data = result.get_data()
    print("Player sprite loaded: " + str(data.player.sprite is Texture2D))
```

## Style Customization with YAMLStyle

Control the formatting and appearance of your YAML output:

```gdscript
# Create a new style configuration
var style = YAML.create_style()

# Set global string style to double-quoted
style.set_string_style(YAMLStyle.STRING_QUOTE_DOUBLE)

# Set integers to display in hexadecimal format
style.set_integer_format(YAMLStyle.INT_HEX)

# Define specific style for player inventory items to use flow style
var inventory_style = style.create_child("inventory")
inventory_style.set_flow_style(YAMLStyle.FLOW_SINGLE)

# Create some data to format
var player_data = {
    "name": "Hero",
    "level": 42,
    "inventory": ["Sword", "Shield", "Potion"]
}

# Apply the style when stringifying
var result = YAML.stringify(player_data, style)
print(result.get_data())

# Output will look like:
# name: "Hero"
# level: 0x2A
# inventory: ["Sword", "Shield", "Potion"]
```

### Style Detection and Preservation

You can detect and preserve the style of existing YAML:

```gdscript
# Parse with style detection
var result = YAML.parse(yaml_text, null, true)

if !result.has_error() and result.has_style():
    var data = result.get_data()
    var style = result.get_style()

    # Modify the data but preserve formatting
    data.player.health = 200

    # Reserialize with the same style
    var output = YAML.stringify(data, style)
    save_file("user://modified_config.yaml", output.get_data())
```

## Examples

Check out the [`examples/`](./addons/yaml/examples/) directory for comprehensive code samples covering:

- Basic parsing and stringification
- Multi-document YAML handling
- Custom class serialization
- **Schema validation workflows**
- Security configurations
- Style customization
- Error handling patterns
- And more!

## Installation

1. Download the plugin from the Godot Asset Library or from the GitHub repository
2. Extract the contents into your project's `addons/` directory
3. Enable the plugin in Project Settings → Plugins

## License

MIT License - See LICENSE file for details.

---

Built with ⚡ by [FimbulWorks](https://github.com/fimbul-works)

# Godot YAML

A high-performance YAML parsing and serialization plugin for Godot 4.3, powered by [RapidYAML](https://github.com/biojppm/rapidyaml). This plugin offers comprehensive YAML support with customizable styling options, full Godot variant type handling, and custom class serialization.

**New to YAML in Godot?** Check out the [`examples/`](./addons/yaml/examples/) directory for comprehensive usage examples covering all features.

## Version History

- **1.1.0** (Current) - YAML files can now be loaded with the `!Resource` tag, and some fixes
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
- 🧪 **Custom Class Serialization**: Register your GDScript classes for seamless serialization and deserialization
- 🔄 **Multi-Document Support**: Parse YAML files with multiple `---` separated documents
- 🎨 **Style Customization**: Control how YAML is formatted with customizable style options
- 🔍 **Comprehensive Error Handling**: Detailed error reporting with line and column information
- 🔀 **Thread-Safe**: Fully supports multi-threaded parsing and emission
- ✅ **Validation**: Separate validation step for checking YAML syntax without full parsing
- 🗂️ **Resource References**: Use `!Resource` tags to reference and load external resources
- 🛡️ **Security Controls**: Manage resource loading security during YAML parsing

## Compatibility

- Requires **Godot 4.3** or higher
- Currently supported platforms:
  - Windows
  - Linux (x86 64-bit)
  - macOS, Android, and iOS support coming soon

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
    var player_data = result.get_data(0)
    var settings = result.get_data(1)

    print("Player: %s (Health: %d)" % [player_data.name, player_data.health])
    print("Difficulty: %s" % settings.difficulty)

    # Check document count
    var doc_count = result.get_document_count()
    print("Found %d documents" % doc_count)
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

## Installation

1. Download the plugin from the Godot Asset Library or from the GitHub repository
2. Extract the contents into your project's `addons/` directory
3. Enable the plugin in Project Settings → Plugins

## License

MIT License - See LICENSE file for details.

---

Built with ⚡ by [FimbulWorks](https://github.com/fimbul-works)

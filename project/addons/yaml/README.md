# Godot YAML

A high-performance YAML parsing and serialization plugin for Godot 4.3, powered by [RapidYAML](https://github.com/biojppm/rapidyaml). This plugin offers comprehensive YAML support with customizable styling options, full Godot variant type handling, and custom class serialization.

## Version History

- **0.12.1** (Current) - Build support for Linux (x86 64-bit)
- **0.12.0** - Performance optimizations, bug fixes, and comprehensive tests for all variant types (see [`examples/variants/`](./examples/variants/))
- **0.11.0** - Added support for parsing multiple documents, and error handling for custom class deserialization
- **0.10.1** - Fixed issue with custom Resources not being serializable
- **0.10.0** - Added custom class serialization support, upgraded to Godot 4.3
- **0.9.0** - Initial public release

## Compatibility

- Requires **Godot 4.3** or higher (previous version 0.9.0 worked with Godot 4.2.2)
- Currently supported platforms:
  - Windows
  - Linux (x86 64-bit)
  - macOS, Android, and iOS support coming soon

## Features

- ⚡ **High Performance**: Built on the lightweight and efficient [RapidYAML](https://github.com/biojppm/rapidyaml) library.
- 🧩 **Full Variant Support** – Handles all\* **Godot built-in Variant types.**
- 🧪 [**Custom Class Support**](#custom-class-serialization): Register your GDScript classes for seamless serialization and deserialization.
- 🗂️ [**Resource References**](#referencing-external-resources) – Use `!Resource` to auto-load scenes, textures, audio, and other assets via `ResourceLoader`.
- 📑 [**Multi-Document Support**](#multi-document-yaml-support) – Parse YAML files with multiple `---` separated documents.
- 🎨 [**Style Customization**](#style-customization): Control how YAML is formatted with customizable style options with `YAMLStyle`.
- 🛡️ [**Security Controls**](#security-controls): Manage resource loading security during YAML parsing.
- 📌 **Tagged Types**: Support for custom YAML tags and automatic tagging of Godot types.
- 🛡️ **Error Handling**: Comprehensive error reporting with line and column information.
- 🧵 **Thread-Safe**: Fully supports multi-threaded parsing and emission without locking.
- 🛡️ [**Validation**](#validation): Separate validation step for checking YAML syntax without full parsing.

<sub>\* Except Callable or RID.</sub>

## Basic Usage

### Parsing YAML

```gdscript
# Parse a YAML string
var yaml_string = """
player:
  name: Hero
  level: 10
  inventory:
    - Sword
    - Shield
"""

var result = YAML.parse(yaml_string)
if result.has_error():
    print("Error: ", result.get_error_message())
else:
    var data = result.get_data()
    print("Player name: ", data.player.name)
```

### Generating YAML

```gdscript
# Convert dictionary to YAML
var data = {
    "name": "Stranger",
    "dialogue": ["Hello, traveler.", "What brings you here?"]
}

var result = YAML.stringify(data)
if !result.has_error():
    var yaml = result.get_data()
    print(yaml)
```

### Working with Files

```gdscript
# Load YAML from a file
var load_result = YAML.load_file("res://data/config.yaml")
if load_result.has_error():
    print("Error loading file: ", load_result.get_error_message())
else:
    var config = load_result.get_data()
    print("Loaded config: ", config)

# Save data to a YAML file
var data = {"settings": {"volume": 0.8, "fullscreen": true}}
var save_result = YAML.save_file(data, "user://settings.yaml")
if save_result.has_error():
    print("Error saving file: ", save_result.get_error_message())
else:
    print("File saved successfully")
```

### Validation

```gdscript
# Validate YAML syntax without full parsing
var yaml_string = "key: value\ninvalid -list"
var validation = YAML.validate(yaml_string)
if validation.has_error():
    print("Invalid YAML: ", validation.get_error_message())
else:
    print("YAML syntax is valid")
```

## Installation

1. Download the plugin from the Godot Asset Library or from the [GitHub repository](https://github.com/fimbul-works/godot-yaml)
2. Extract the contents into your project's `addons/` directory
3. Enable the plugin in Project Settings → Plugins

## Supported Types

The plugin automatically handles conversion between YAML and all standard Godot variant types:

- Basic types: `bool`, `int`, `float`, `String`, `StringName`
- Collection types: `Array`, `Dictionary`
- Vector types: `Vector2`, `Vector2i`, `Vector3`, `Vector3i`, `Vector4`, `Vector4i`
- Transform types: `Transform2D`, `Transform3D`, `Projection`
- Geometric types: `AABB`, `Basis`, `Plane`, `Quaternion`, `Rect2`, `Rect2i`
- Color type: `Color`
- Array types: `PackedByteArray`, `PackedColorArray`, `PackedFloat32Array`, `PackedFloat64Array`, `PackedInt32Array`, `PackedInt64Array`, `PackedStringArray`, `PackedVector2Array`, `PackedVector3Array`
- Reference types: `NodePath`, `Resource` (see [Resource references](#referencing-external-resources))
- **Custom GDScript classes** (with registration)
- Unknown YAML types are safely converted to strings or dictionaries, ensuring no data loss

### Referencing External Resources
You can use the `!Resource` tag to automatically load external resources using `ResourceLoader.load()`. This allows YAML files to reference any loadable Godot resource, like scenes, textures, audio files, and more.

```gdscript
var yaml_string = """
sprite:
  texture: !Resource res://textures/hero.png
  scene: !Resource res://scenes/npc.tscn
"""

var result = YAML.parse(yaml_string)
var data = result.get_data()

# These will be actual loaded Resource instances
print(data.sprite.texture is Texture2D)  # true
print(data.sprite.scene is PackedScene)  # true
```

## Custom Class Serialization

You can register your custom GDScript classes for seamless serialization:

```gdscript
# Define a custom class
class_name Player extends Node

var name: String
var level: int
var inventory: Array

static func deserialize(dict) -> Player:
    var player = Player.new()
    player.name = dict.get("name", "")
    player.level = dict.get("level", 1)
    player.inventory = dict.get("inventory", [])
    return player

func serialize() -> Dictionary:
    return {
        "name": name,
        "level": level,
        "inventory": inventory
    }

# Register the class with YAML
func _ready() -> void:
    YAML.register_class(Player)

    # Now you can serialize and deserialize Player objects
    var player = Player.new()
    player.name = "Hero"
    player.level = 10
    player.inventory = ["Sword", "Shield"]

    var data = {"player": player}
    var yaml = YAML.stringify(data).get_data()
    print(yaml)

    # Deserialize from YAML
    var parsed = YAML.parse(yaml).get_data()
    var restored_player = parsed.player
    print(restored_player.name)  # Hero
```

### Error Handling for Custom Classes

You can add validation and return detailed error messages from your `deserialize` method by returning a `YAMLResult` object:

```gdscript
static func deserialize(data: Variant):
    # Validate data type
    if typeof(data) != TYPE_DICTIONARY:
        return YAMLResult.error("Deserializing Player expects Dictionary")

    # Validate required fields
    if !data.has("name"):
        return YAMLResult.error("Player class missing required 'name' field")

    # Create object if validation passes
    var player = Player.new()
    player.name = data.get("name", "")
    player.level = data.get("level", 1)
    return player
```

## Security Controls

The plugin provides security controls for resource loading via the `YAMLSecurity` class. This helps protect against potential security vulnerabilities when loading YAML from untrusted sources.

```gdscript
# Create custom security settings
var security = YAML.create_security()

# Allow resources from specific directories
security.allow_path("res://assets/")
security.allow_path("res://textures/", ["Texture2D", "Image"])

# Block specific resource types
security.block_type("Script")
security.block_type("AudioStreamMP3")

# Parse YAML with custom security settings
var yaml_string = """
player:
  sprite: !Resource res://assets/player.png
  script: !Resource res://scripts/player.gd
"""
var result = YAML.parse(yaml_string, false, security)
```

### Default Security

By default, Script and GDExtension resource types are blocked for security reasons. You can also use the static methods of the YAML class for simple security management:

```gdscript
# Using the default security instance
YAML.allow_resource_path("res://assets/")
YAML.block_resource_type("AudioStreamMP3")

# Reset to defaults (blocks only Script and GDExtension)
YAML.reset_security()
```

## Style Customization

YAML output can be customized using the `YAMLStyle` class:

```gdscript
# Create a new style
var style = YAML.create_style()

# Set scalar style (SCALAR_PLAIN, SCALAR_BLOCK, SCALAR_LITERAL, SCALAR_FOLDED)
style.set_scalar_style(YAMLStyle.SCALAR_LITERAL)  # Use | for multiline strings

# Set quote style (QUOTE_NONE, QUOTE_SINGLE, QUOTE_DOUBLE)
style.set_quote_style(YAMLStyle.QUOTE_DOUBLE)     # Use " for strings

# Set flow style (FLOW_NONE, FLOW_SINGLE)
style.set_flow_style(YAMLStyle.FLOW_SINGLE)       # Use [] and {} style

# Apply style to specific child nodes
var nested_style = YAML.create_style()
nested_style.set_flow_style(YAMLStyle.FLOW_NONE)  # Use block style for this child
style.set_child("nested", nested_style)

# Apply the style when generating YAML
var data = {
    "string": "Hello\nWorld",
    "nested": {"list": [1, 2, 3]}
}
var yaml = YAML.stringify(data, style).get_data()
print(yaml)

# Apply style to file output
var file_result = YAML.save_file(data, "user://styled_output.yaml", style)
```

### Style Detection

You can automatically detect and preserve the styling of parsed YAML:

```gdscript
# Parse with style detection enabled
var yaml_string = """
list:
  - item1
nested: {key1: value1}  # Flow style
multiline: |            # Literal style
  This is a multiline
  string
"""

var result = YAML.parse(yaml_string, true)  # Enable style detection
if !result.has_error() && result.has_style():
    var data = result.get_data()
    var style = result.get_style()

    # Modify data while preserving style
    data.list.append("item2")

    # Re-emit with preserved style
    var output = YAML.stringify(data, style).get_data()
    print(output)

    # Or save to file with preserved style
    YAML.save_file(data, "user://preserved_style.yaml", style)
```

## Multi-Document YAML Support

The plugin fully supports YAML files containing multiple documents separated by `---` delimiters.

```gdscript
# Parse a multi-document YAML string
var yaml_string = """
# First document
title: Document 1
---
# Second document
title: Document 2
"""

var result = YAML.parse(yaml_string)
if !result.has_error():
    # Get number of documents
    var doc_count = result.get_document_count()
    print("Number of documents: ", doc_count)  # Output: 2

    # Access documents by index
    var first_doc = result.get_document(0)
    var second_doc = result.get_document(1)

    print(first_doc.title)   # Output: Document 1
    print(second_doc.title)  # Output: Document 2
```

## Error Handling and Troubleshooting

### Common Error Types

- **Syntax Errors**: Invalid YAML structure detected during parsing
- **Type Conversion Errors**: When YAML values can't be converted to expected types
- **Circular Reference Errors**: When data structures contain circular references
- **File Access Errors**: When reading from or writing to files fails

### Troubleshooting Tips

1. **Check Indentation**: YAML is sensitive to indentation. Use spaces consistently.
2. **Validate First**: Use `YAML.validate()` to check syntax before full parsing.
3. **Inspect Error Details**: Always check `get_error_line()` and `get_error_column()` to pinpoint issues.
4. **Escape Special Characters**: Use quotes when values contain special characters.

## Reporting Issues and Contributing

- **Bug Reports**: Please use the [GitHub issue tracker](https://github.com/fimbul-works/godot-yaml/issues).
- **Feature Requests**: Feel free to suggest improvements through GitHub issues.
- **Contributing**: Pull requests are welcome! See the main repository README for development guidelines.

## Credits

- Powered by [RapidYAML](https://github.com/biojppm/rapidyaml) (ryml) - an efficient C++ library for YAML processing.

## License

MIT License - See [LICENSE](LICENSE) file for details.

---

Built with ⚡ by [FimbulWorks](https://github.com/fimbul-works)

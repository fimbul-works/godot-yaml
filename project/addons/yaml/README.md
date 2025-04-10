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
    - Potion
"""

var result = YAML.parse(yaml_string)
if result.has_error():
    print("Error: ", result.get_error_message())
    print("At line: ", result.get_error_line(), ", column: ", result.get_error_column())
else:
    var data = result.get_data()
    print("Player name: ", data.player.name)
    print("Inventory: ", data.player.inventory)
```

### Generating YAML

```gdscript
# Create a dictionary to convert to YAML
var data = {
    "name": "Stranger",
    "dialogue": ["Hello, traveler.", "What brings you here?"]
}

# Convert to YAML
var stringify_result = YAML.stringify(data)
if stringify_result.has_error():
    print("Error: ", stringify_result.get_error_message())
else:
    var yaml = stringify_result.get_data()
    # name: Stranger
    # dialogue:
    #   - "Hello, traveler."
    #   - "What brings you here?"
    print(yaml)
```

### Validation

```gdscript
# Validate YAML syntax without full parsing
var yaml_string = "key: value\ninvalid -list"
var validation = YAML.validate(yaml_string)
if validation.has_error():
    print("Invalid YAML: ", validation.get_error_message())
    print("At line: ", validation.get_error_line(), ", column: ", validation.get_error_column())
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

## Using Helper Classes

The plugin provides convenient helper classes for common operations:

### YAMLLoader

```gdscript
# Load YAML from a file
var data = YAMLLoader.load_file("res://data.yaml")
if YAMLLoader.last_error != null:
    print("Error loading file: ", YAMLLoader.last_error)
else:
    print("Loaded data: ", data)

# Load from string
var yaml_string = "key: value\nlist: [1, 2, 3]"
data = YAMLLoader.load_string(yaml_string)
```

### YAMLWriter

```gdscript
# Save data to a YAML file
var data = {"key": "value", "list": [1, 2, 3]}
var success = YAMLWriter.save_file(data, "user://output.yaml")
if !success:
    print("Error saving file: ", YAMLWriter.last_error)

# Convert to YAML string
var yaml_string = YAMLWriter.save_string(data)
print(yaml_string)
```

## Custom Class Serialization

You can register your custom GDScript classes for seamless serialization:

```gdscript
# Define a custom class
class_name Player extends Node

var name: String
var level: int
var inventory: Array

func _init(p_name: String = "", p_level: int = 1) -> void:
    name = p_name
    level = p_level
    inventory = []

static func from_dict(dict: Dictionary) -> Player:
    var player = Player.new(dict.get("name", ""), dict.get("level", 1))
    player.inventory = dict.get("inventory", [])
    return player

func to_dict() -> Dictionary:
    return {
        "name": name,
        "level": level,
        "inventory": inventory
    }

# Register the class with YAML
func _ready() -> void:
    # Default registration using to_dict() and from_dict() methods
    YAML.register_class(Player)

    # Or with custom method names
    # YAML.register_class(Player, "serialize", "deserialize")

    # Now you can serialize and deserialize Player objects
    var player = Player.new("Hero", 10)
    player.inventory = ["Sword", "Shield", "Potion"]

    var data = {"player": player}

    # Serialize to YAML
    var yaml = YAML.stringify(data).get_data()
    print(yaml)
    # Output:
    # player: !Player
    #   name: Hero
    #   level: 10
    #   inventory:
    #     - Sword
    #     - Shield
    #     - Potion

    # Deserialize from YAML
    var parsed = YAML.parse(yaml).get_data()
    var restored_player = parsed.player
    print(restored_player.name)  # Hero
```

### Error Handling for Custom Classes

You can add validation and return detailed error messages from your `from_dict` method by returning a `YAMLResult` object:

```gdscript
static func from_dict(dict: Dictionary):
    # Validate required fields
    if !dict.has("name"):
        return YAMLResult.error("Player class missing required 'name' field")

    if typeof(dict.get("level", 0)) != TYPE_INT:
        return YAMLResult.error("Player 'level' must be an integer")

    # Create object if validation passes
    var player = Player.new(dict.name, dict.get("level", 1))
    player.inventory = dict.get("inventory", [])

    # Return the object as usual
    return player
```

When parsing YAML with a custom class that returns a `YAMLResult`:

- If `from_dict` returns a `YAMLResult` with an error, the entire parse operation will fail
- The error message from your custom class will be propagated to the parse result
- This allows for detailed validation errors with custom messages

Example:

```gdscript
var invalid_yaml = """
player: !Player
  # Missing name field
  level: ten  # Invalid type
"""

var result = YAML.parse(invalid_yaml)
if result.has_error():
    print("Error: " + result.get_error_message())
    # Output: "Error: Player 'level' must be an integer"
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

# Set number format (NUM_DECIMAL, NUM_HEX, NUM_OCTAL, NUM_BINARY, NUM_SCIENTIFIC)
style.set_number_format(YAMLStyle.NUM_HEX)        # Output numbers in hex format

# Apply style to specific child nodes
var nested_style = YAML.create_style()
nested_style.set_flow_style(YAMLStyle.FLOW_NONE)  # Use block style for this child
style.set_child("nested", nested_style)

# Apply the style when generating YAML
var data = {
    "string": "Hello\nWorld",
    "nested": {
        "list": [1, 2, 3],
        "mapping": {"a": 1, "b": 2}
    }
}
var result = YAML.stringify(data, style)
var yaml = result.get_data()
print(yaml)
```

### Style Detection

You can automatically detect and preserve the styling of parsed YAML:

```gdscript
# Parse with style detection enabled
var yaml_string = """
list:
  - item1
  - item2
nested: {key1: value1, key2: value2}  # Flow style
multiline: |                          # Literal style
  This is a multiline
  string that preserves
  line breaks
"""

var result = YAML.parse(yaml_string, true)  # Enable style detection
if !result.has_error() && result.has_style():
    var data = result.get_data()
    var style = result.get_style()

    # Modify data while preserving style
    data.list.append("item3")

    # Re-emit with preserved style
    var output = YAML.stringify(data, style).get_data()
    print(output)
```

## Multi-Document YAML Support

The plugin fully supports YAML files containing multiple documents separated by `---` delimiters. This is particularly useful for any scenario where related but separate data structures need to be kept together.

### Parsing Multiple Documents

```gdscript
# Parse a multi-document YAML string
var yaml_string = """
# First document
title: Document 1
data:
  - item1
  - item2
---
# Second document
title: Document 2
config:
  enabled: true
---
# Third document
title: Document 3
message: "Final document"
"""

var result = YAML.parse(yaml_string)
if result.has_error():
    print("Error: ", result.get_error_message())
else:
    # Get number of documents
    var doc_count = result.get_document_count()
    print("Number of documents: ", doc_count)  # Output: 3

    # Process each document
    for i in range(doc_count):
        var doc = result.get_document(i)
        print("Document %d title: %s" % [i, doc.title])

    # You can also access documents directly by index
    var first_doc = result.get_document(0)   # First document
    var second_doc = result.get_document(1)  # Second document

    # The original get_data() method still works, now with optional index
    var third_doc = result.get_data(2)       # Third document
```

### Notes on Multi-Document Usage

- Documents are indexed starting from `0`
- For backward compatibility, `get_data()` without an index returns the first document
- `get_document_count()` returns the number of documents (at least `1` for valid YAML), or `0` if the result has an error
- When parsing single-document YAML, `get_document(0)` and `get_data()` return identical results

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

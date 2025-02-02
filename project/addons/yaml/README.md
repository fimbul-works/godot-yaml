# Godot YAML Plugin

A high-performance YAML parsing and serialization plugin for Godot 4.2.2 or higher, powered by [RapidYAML](https://github.com/biojppm/rapidyaml). Features syntax highlighting, style customization, and comprehensive type support. Capable of sub-millisecond parsing and emitting of complex YAML documents.

## Features

- ⚡ **Blazing Fast**: Sub-millisecond parsing and emitting powered by RapidYAML (ryml)
- 📄 **Full YAML Support**: Parse and generate YAML-compliant documents including anchors, aliases, tags, and complex data types
- 🎨 **Syntax Highlighting**: Built-in syntax highlighting for .yaml/.yml files in the editor
- ✅ **Live Validation**: Real-time error checking and validation while editing
- 🎭 **Style Customization**: Control YAML output formatting including scalar styles, flow styles, and more
- 🔄 **Type Support**: Comprehensive support for Godot variant types including vectors, transforms, and packed arrays
- 🛠️ **High-Level API**: Convenient wrapper classes for common operations

## Installation

1. Create an `addons` directory in your project if it doesn't exist
2. Copy the `yaml` directory into `addons`
3. Enable the plugin in Project Settings -> Plugins

### Platform Support

Currently supported platforms:
- ✅ Windows (x86_32, x86_64)

Coming soon:
- 🚧 Linux
- 🚧 macOS
- 🚧 Android
- 🚧 iOS

Want to help? We welcome contributions for building and testing on additional platforms!

## Basic Usage

```gdscript
# Parse YAML string
var result = YAML.parse(yaml_string)
if !result.has_error():
    var data = result.get_data()

# Generate YAML string
var stringify_result = YAML.stringify(data)
if !stringify_result.has_error():
    var yaml = stringify_result.get_data()
```

### Using YAMLLoader/YAMLWriter

Convenience classes for common operations:

```gdscript
# Load from file
var data = YAMLLoader.load_file("res://data.yaml")

# Save to file
YAMLWriter.save_file(data, "user://output.yaml")
```

### Style Customization

```gdscript
# Create and customize style
var style = YAML.create_style()
style.set_scalar_style(YAMLStyle.SCALAR_LITERAL)  # Use | for multiline strings
style.set_quote_style(YAMLStyle.QUOTE_DOUBLE)     # Use " for strings

# Apply style when generating YAML
var yaml = YAMLWriter.save_string(data, style)
```

## Examples

The plugin includes several example scripts in `addons/yaml/examples/` demonstrating various features:

- `basic_usage.gd`: Core parsing and stringification
- `error_handling.gd`: Error handling and validation
- `loader_writer.gd`: High-level API usage
- `speed_benchmark.gd`: Performance testing
- `style_handling.gd`: Style customization features
- `variant_types.gd`: Godot type support

## API Reference

### YAML Singleton

The main interface for YAML operations:

- `parse(yaml: String, detect_style: bool = false) -> YAMLResult`
- `stringify(data: Variant, style: YAMLStyle = null) -> YAMLResult`
- `validate(yaml: String) -> YAMLResult`
- `create_style() -> YAMLStyle`
- `version() -> String`

### YAMLLoader

Convenience class for loading YAML content:

- `load_string(yaml_str: String) -> Variant`
- `load_file(path: String) -> Variant`

### YAMLWriter

Convenience class for saving YAML content:

- `save_string(data: Variant, style: YAMLStyle = null) -> String`
- `save_file(data: Variant, path: String, style: YAMLStyle = null) -> bool`

## Known Issues

- Stringifying/parsing the `Callable` variant is not supported
- Stringifying/parsing `Object` types is not supported
- Stringifying/parsing *local* `Resource` types is not supported
- Parsing floating point values can sometimes have slight variance in the output

## License

MIT License (see [LICENSE](LICENSE) file for details)

# Godot YAML

Godot YAML is a GDExtension for Godot 4.2.2 and higher that adds YAML parsing and stringification capabilities to your Godot projects. This extension wraps the [Rapid YAML](https://github.com/biojppm/rapidyaml) library, providing an easy-to-use interface for working with YAML in Godot.

It currently has support for Windows, but should compile on other platforms.

## Features

- Parse YAML strings into Godot Dictionaries
- Stringify Godot Dictionaries into YAML format
- Supports Godot 4.2.2 and higher

## Installation

1. Download the latest release from the [Releases](https://github.com/claus-codes/godot-yaml/releases) page.
2. Extract the `yaml` folder into your Godot project's `addons/` directory.
3. The folder structure should look like this:
   ```
   your_project/
   └── addons/
       └── yaml/
           ├── bin/
           ├── yaml.gdextension
           ├── plugin.cfg
           ├── plugin.gd
           └── README.md
   ```
4. Godot will automatically detect and load the extension when you open your project.

## Usage

Here's a basic example of how to use Godot YAML in your Godot script:

```gdscript
var yaml = YAML.new()

# Parsing YAML
var yaml_string = """
key: value
list:
  - item1
  - item2
"""
var parsed_data = yaml.parse(yaml_string)
print(parsed_data)

# Stringifying to YAML
var dict_to_stringify = {
    "key": "value",
    "list": ["item1", "item2"]
}
var yaml_output = yaml.stringify(dict_to_stringify)
print(yaml_output)

# Check for errors
if yaml.get_error() != null:
  printerr(yaml.get_error())

# Getting version information
print(yaml.version())
```

## Demo Project

A demo project is included in the `/project` folder of this repository. It provides examples of how to use the Godot YAML extension in a Godot project. To run the demo:

1. Open Godot 4.2.2 or higher
2. Use "Import" and select the `project.godot` file in the `/project` folder
3. Once imported, run the project to see Godot YAML in action

## Building from Source

If you want to build the extension from source:

1. Clone this repository
2. Make sure you have SCons installed
3. Run `scons` in the root directory of the project
4. The compiled libraries will be in the `/project/addons/yaml/bin` directory

## License

See the [LICENSE](LICENSE) file for details.

## Acknowledgments

- [Rapid YAML](https://github.com/biojppm/rapidyaml) - The YAML parser and emitter library used in this project
- [Godot Engine](https://godotengine.org/) - The game engine this extension is built for

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## Support

If you encounter any issues or have questions, please file an issue on the [GitHub issue tracker](https://github.com/claus-codes/godot-yaml/issues).

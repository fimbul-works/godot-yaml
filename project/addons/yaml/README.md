# Godot YAML

Godot YAML is a GDExtension for Godot 4.2.2 and higher that adds YAML parsing and stringification capabilities to your Godot projects.

## Installation

1. Extract this `yaml` folder into your Godot project's `addons/` directory.
2. Your project structure should look like this:
   ```
   your_project/
   └── addons/
       └── yaml/
           ├── bin/
           │   └── [platform-specific libraries]
           ├── yaml.gdextension
           ├── plugin.cfg
           ├── plugin.gd
           └── README.md
   ```
3. Open your Godot project and go to Project > Project Settings > Plugins.
4. Find "YAML" in the list and check the "Enable" box.
5. Godot will now load the extension when you run your project.

## Basic Usage

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
if yaml.get_error() != null
  printerr(yaml.get_error())

# Getting version information
print(yaml.version())
```

## More Information

For more detailed information, examples, and the latest updates, please visit the [Godot YAML GitHub repository](https://github.com/claus-codes/godot-yaml).

## Support

If you encounter any issues or have questions, please file an issue on the [GitHub issue tracker](https://github.com/claus-codes/godot-yaml/issues).

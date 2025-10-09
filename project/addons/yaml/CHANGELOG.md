# Godot YAML Changelog

## Version 2.1.1

Empty strings are stringified with quotes.

## Version 2.1.0

When passing the custom tag to `YAML.schema_register(class, serialize_method, deserialize_static, custom_tag)` use the custom tag when emitting, resolving [Issue #13](https://github.com/fimbul-works/godot-yaml/issues/13).

## Version 2.0.0

The second major version of Godot YAML brings powerful schema validation capabilities through the integration of [GDSchema](https://github.com/fimbul-works/gdschema), enabling [JSON Schema Draft-7](https://json-schema.org/) validation for your YAML data. This release also includes improved multi-document handling, custom tag support, and several important bug fixes. See the [migration guide](#migration-guide-from-version-1-to-200) for breaking changes.

### Major Features

#### Schema Validation (New!)

Version 2.0.0 introduces comprehensive schema validation powered by [GDSchema](https://github.com/fimbul-works/gdschema), bringing the full power of [JSON Schema Draft-7](https://json-schema.org/) to your YAML workflows. This flagship feature enables you to:

- **Validate YAML data** against industry-standard JSON Schema specifications
- **Define schemas in YAML** with native YAML syntax for better readability
- **Auto-apply defaults** when properties are missing using the `default` keyword
- **Validate YAML tags** with the custom `x-yaml-tag` keyword for type safety
- **Reference schemas** using `$ref` for modular, reusable validation rules
- **Register schemas globally** with `$id` for cross-file validation
- **Get detailed error reports** with JSON Pointer paths and constraint information

New classes:
- `Schema` - Validates data against JSON Schema Draft-7 specifications
- `SchemaValidationResult` - Contains detailed validation results and errors

New YAML methods:
- `YAML.load_schema_from_string()` - Parse YAML schema definitions
- `YAML.load_schema_from_file()` - Load schema files with auto-registration
- `YAML.parse_and_validate()` - Parse and validate YAML in one step

Schema features include:
- Full JSON Schema Draft-7 support (type checking, numeric constraints, string patterns, array/object validation)
- Logical composition (`allOf`, `anyOf`, `oneOf`, `not`)
- Conditional validation (`if`/`then`/`else`)
- Schema definitions and references (`$defs`, `$ref`)
- Custom format validators
- Thread-safe lazy compilation with caching
- YAML-specific extensions:
  - `default` - Provides default values for missing properties
  - `x-yaml-tag` - Validates YAML type tags (e.g., `!Resource`, `!CustomType`)

Example:
```gdscript
# Define a schema in YAML
var schema_yaml = """
$id: "http://example.com/user.yaml"
type: object
properties:
  username:
    type: string
    minLength: 3
  email:
    type: string
    format: email
  role:
    type: string
    default: user
    x-yaml-tag: UserRole
required:
- username
- email
"""

# Build the schema (auto-registered with $id)
var schema = YAML.load_schema_from_string(schema_yaml)

# Validate data
var data_yaml = """
username: Alice
email: alice@exampe.com
"""

var result = YAML.parse_and_validate(data_yaml, schema)

if result.is_valid():
    print(result.get_data().role)  # "user" (default applied)
else:
    print(result.get_summary())  # Detailed error report
```

### Other Changes

- **Breaking:** Removed the `YAMLResult.get_data(index)` parameter. Use `YAMLResult.get_document(index)` instead for multi-document access.
- Added `YAMLResult.get_documents()` and `YAMLResult.has_multiple_documents()` methods for easier multi-document YAML handling
- Added support for custom tags when registering classes with `YAML.register_class()` - now you can specify custom YAML tags like `!Item` or `!ruby:object/Item`
- Added `YAMLCodeEdit` class to enable embedding the YAML editor directly into your projects
- Added missing example for handling multi-document YAML files

### Bug Fixes

- Fixed `YAMLResult.get_data()` only returning the first element of a document that only contains an array
- Fixed invalid numeric value parsing that could cause crashes or incorrect values
- Fixed empty strings being parsed and emitted as null values instead of empty strings
- Fixed YAML files not opening correctly in Godot 4.3 editor

### Migration Guide From Version 1.* to 2.0.0

Replace `YAMLResult.get_data(index)` calls with `YAMLResult.get_document(index)`:

**Old version:**
```gdscript
var result = YAML.parse(multi_doc_yaml)
var first_doc = result.get_data(0)
var second_doc = result.get_data(1)
```

**New version:**
```gdscript
var result = YAML.parse(multi_doc_yaml)
var first_doc = result.get_document(0)
var second_doc = result.get_document(1)

# Or get all documents at once
var all_docs = result.get_documents()
```

## Version 1.1.0

- YAML files can now be loaded with the `!Resource` tag, allowing modular composition
- Fixes:
    - Force YAML indentation to always use spaces instead of tabs
    - The file system should now update properly when saving a new YAML file
    - Fixed tab indentation not working correctly in the YAML editor
    - Fixed Packed Array types not detecting array templates
    - Prevent duplicates in YAML editor file list

## Version 1.0.0

The first major version of Godot YAML includes many improvements, and some breaking changes. See the [migration guide](#migration-guide-from-version-0121-to-100) for details.

### Changes

- Streamlined API in many places
- Added class documentation for the in-editor help
- Added custom YAML editor inside the Godot engine editor
- Added `YAMLSecurity` class for safer resource loading during parsing
- `YAMLStyle` API changes:
  - Adopted Godot terminology for container form (Array and Dictionary instead of Sequence and Map)
  - Merged scalar and quote styles into unified string style
  - Separated number format into integer and float formats respectively
  - Removed binary encoding (now always uses base64 encoding)
  - Implemented serializing/deserializing style definitions
- Implemented stringifying non-local-to-scene resource references
- Performance optimizations
- Improved error reporting across the board with line/column tracking when parsing
- Fixes:
  - Style definitions should now be parsed correctly
  - Stringifying with styles should now work consistently
  - Fixed custom tags not being used when stringifying with styles
  - Fixed integer to string conversion with 64 bit values

### Migration Guide From Version 0.12.1 to 1.0.0

This migration guide should help you update your code to work with the latest version of the YAML plugin. The core functionality and architecture remain similar, but the style system has been refined for better consistency and usability.

#### Custom Class Serialization and Deserialization

Custom class serialization has been changed to allow serializing to/from just dictionaries to any of the supported Godot variants:

**Old version:**
```gdscript
class_name CustomClass extends Resource

@export var name: String

func _init(p_name: String = "") -> void:
    name = p_name

func to_dict() -> Dictionary:
    return {
      "name": name
    }

static func from_dict(data: Dictionary) -> CustomClass:
    return CustomClass.new(data["name"])
```

**New version:**
```gdscript
class_name CustomClass extends Resource

@export var name: String

func _init(p_name: String = "") -> void:
    name = p_name

func serialize() -> Variant:
    return {
      "name": name
    }

static func deserialize(data: Variant) -> Variant:
    if typeof(data) != TYPE_DICTIONARY:
        return YAMLResult.error("CustomClass expects Dictionary")

    return CustomClass.new(data["name"])
```

#### Container Form Changes

Previously the container forms used the YAML standard naming of *Sequences* and *Maps*, but to make the extension more Godot friendly the containers now follow Godot's naming conventions of *Arrays* and *Dictionaries*:

```gdscript
# Specify how containers are formatted
style.set_container_form(YAMLStyle.FORM_ARRAY)      # Previously FORM_SEQ
style.set_container_form(YAMLStyle.FORM_DICTIONARY) # Previously FORM_MAP
```

#### Scalar Style and Quote Style Merged

**Old version:**
```gdscript
var style = YAML.create_style()
# Separate methods for different aspects of string formatting
style.set_scalar_style(YAMLStyle.SCALAR_LITERAL) # For block style (|)
style.set_quote_style(YAMLStyle.QUOTE_DOUBLE)    # For quoting style
```

**New version:**
```gdscript
var style = YAML.create_style()
# Single method combining both formatting aspects
style.set_string_style(YAMLStyle.STRING_LITERAL) # For block style (|)
style.set_string_style(YAMLStyle.STRING_QUOTE_DOUBLE) # For quoting style
```

The constants have been unified:
- `YAMLStyle.SCALAR_PLAIN`, `YAMLStyle.SCALAR_LITERAL`, `YAMLStyle.SCALAR_FOLDED` are now accessible through the `STRING_*` constants
- `YAMLStyle.QUOTE_NONE`, `YAMLStyle.QUOTE_SINGLE`, `YAMLStyle.QUOTE_DOUBLE` have been replaced with `STRING_PLAIN`, `STRING_QUOTE_SINGLE`, `STRING_QUOTE_DOUBLE`

#### Number Format Changes

**Old version:**
```gdscript
style.set_number_format(YAMLStyle.NUM_HEX)
```

**New version:**
```gdscript
style.set_integer_format(YAMLStyle.INT_HEX)
style.set_float_format(YAMLStyle.FLOAT_SCIENTIFIC)
```

Integer and float formatting have been split into two methods with dedicated enum values:
- `YAMLStyle.NUM_DECIMAL`, `YAMLStyle.NUM_HEX`, etc. are now `YAMLStyle.INT_DECIMAL`, `YAMLStyle.INT_HEX`, etc.
- `YAMLStyle.NUM_SCIENTIFIC` is now `YAMLStyle.FLOAT_SCIENTIFIC`

#### Removed YAMLLoader and YAMLWriter Classes

If using the old loaders, they have been integrated to the main `YAML` class:

**Old version:**
```gdscript
# Load YAML from a file
var data = YAMLLoader.load_file("res://data.yaml")
if YAMLLoader.last_error != null:
    print("Error loading file: ", YAMLLoader.last_error)
else:
    print("Loaded data: ", data)

# Save data to a YAML file
var data = {"key": "value", "list": [1, 2, 3]}
var success = YAMLWriter.save_file(data, "user://output.yaml")
if !success:
    print("Error saving file: ", YAMLWriter.last_error)
```

**New version:**
```gdscript
# Load YAML from a file
var load_result = YAML.load_file("res://example.yaml")
if load_result.has_error():
    push_error(load_result.get_error())
    return null
var data = load_result.get_data()
print("Loaded data: ", data)

# Save data to a YAML file
var save_result = YAML.save_file(data, "user://output.yaml")
if save_result.has_error():
    push_error(save_result.get_error())
    return null
var yaml_text = save_result.get_data()
print("Saved YAML:\n", yaml_text)

# The above can also be written as
var null_on_fail = YAML.try_load_file("res://example.yaml")
var success = YAML.try_save_file(data, "user://output.yaml")
```

## Older Versions

- **0.12.1** - Build support for Linux (x86 64-bit)
- **0.12.0** - Performance optimizations, bug fixes, and comprehensive tests for all variant types
- **0.11.0** - Added support for parsing multiple documents, and error handling for custom class deserialization
- **0.10.1** - Fixed issue with custom Resources not being serializable
- **0.10.0** - Added custom class serialization support, upgraded to Godot 4.3
- **0.9.0** - Initial public release

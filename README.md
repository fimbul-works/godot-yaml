# Godot YAML GDExtension

This is the **C++ GDExtension implementation** of the Godot YAML plugin. It provides **high-performance YAML parsing and serialization** using [RapidYAML](https://github.com/biojppm/rapidyaml) as the core engine, offering **sub-millisecond processing** for most YAML documents. This extension is built for Godot 4.3 or later.

## Version History

- **2.2.1** (Current) - Upgraded GDSchema to 2.0.1, and fixed multiple memory leaks
- **2.2.0** - Upgraded GDSchema to 2.0.0 for JSON validation draft 2020-12 spec, and various bug fixes
- **2.1.2** - Added support for Android (arm64, x86_64) platforms thanks to @Nemo1166, and fixed some bugs
- **2.1.1** - Empty strings are stringified with quotes
- **2.1.0** - When passing custom tag to `YAML.schema_register(class, serialize_method, deserialize_static, custom_tag)` use the custom tag when stringifying
- **2.0.0** - Major release with schema validation powered by [GDSchema](https://github.com/fimbul-works/gdschema), improved multi-document handling, and bug fixes.

See [the full changelog](./project/addons/yaml/CHANGELOG.md#version-200) for details of earlier versions.

## Quick Start

```gdscript
# Parse YAML
var result = YAML.parse("key: value\nlist:\n  - item1\n  - item2")
if result.has_error():
    push_error(result.get_error())
    return

var data = result.get_data()
print(data.key)  # Outputs: value
print(data.list) # Outputs: [item1, item2]

# Generate YAML
var yaml_text = YAML.stringify(data).get_data()
print(yaml_text)
```

📌 **For full usage details and API documentation, see the plugin README:**
📂 [`project/addons/yaml/README.md`](project/addons/yaml/README.md)

## Key Features

- ⚡ **High Performance**: Built on the lightweight and efficient [RapidYAML](https://github.com/biojppm/rapidyaml) library
- 🧩 **Comprehensive Variant Support**: Handles all Godot built-in Variant types (except Callable and RID)
- ✅ **Schema Validation**: Full JSON Schema Draft 2020-12 validation powered by [GDSchema](https://github.com/fimbul-works/gdschema) with YAML-specific extensions
- 🧪 **Custom Class Serialization**: Register your GDScript classes for seamless serialization and deserialization
- 📄 **Multi-Document Support**: Parse YAML files with multiple `---` separated documents
- 🎨 **Style Customization**: Control how YAML is formatted with customizable style options
- 📝 **Comprehensive Error Handling**: Detailed error reporting with line and column information
- 🔀 **Thread-Safe**: Fully supports multi-threaded parsing and emission
- 🗂️ **Resource References**: Use `!Resource` tags to reference and load external resources
- 🛡️ **Security Controls**: Manage resource loading security during YAML parsing

## Installation & Setup

Godot YAML is available on the [Godot Asset Library](https://godotengine.org/asset-library/asset/3774). Be sure to enable the plugin in your project settings after installing.

### Building From Source

Alternatively you can build the binaries yourself. For the [latest versions of macOS](#supported-platforms) this may be necessary.

#### Prerequisites
- **Git** (for cloning and submodules)
- **Python 3.x** (for SCons build system)
- **C++ compiler** with C++17 support:
  - **Windows**: Visual Studio 2022 with C++ workload
  - **Linux/macOS**: GCC 9+ or Clang 10+
- **SCons** build system (`pip install scons`)
- **Android**: JDK version 20, NDK version 23.2.8568313, and Ninja

#### Step 1: Clone the Repository
```bash
# Clone with submodules
git clone --recursive https://github.com/fimbul-works/godot-yaml

# Or if already cloned, initialize submodules
git submodule update --init --recursive
```

#### Step 2: Build the Extension
```bash
# Debug build
scons target=template_debug

# Release build
scons target=template_release

# Specify platform (default is platform-dependent)
scons platform=windows target=template_release
scons platform=linux target=template_release
```

#### Build Options
- `platform`: Target (`windows`, `linux`, `macos`, `android`, etc.)
- `target`: Build type (`template_debug`, `template_release`)
- `arch`: CPU architecture (`x86_32`, `x86_64`, `arm64`, etc.)
- `dev_build`: Enable extra debugging (`yes`/`no`)
- `use_llvm`: Use Clang/LLVM compiler (`yes`/`no`)
- `verbose`: Verbose build output (`yes`/`no`)

---

## Supported Platforms

- **Windows**: ✅ Prebuilt binaries available for **x86 64-bit** architecture.
- **Linux**: ✅ Prebuilt binaries available for **x86 64-bit** architecture.
- **macOS**: ⚠️ Prebuilt binaries available for **universal** architecture.
  - **Note**: Some macOS configurations (particularly newer versions with stricter Gatekeeper policies) may prevent loading of GDExtensions generally, not just this plugin. If the extension fails to load, try building from source or test with other GDExtensions to determine if this is a system-wide issue.
- **Android**: ✅ Prebuilt binaries available for **ARM 64-bit** and **x86 64-bit** architectures.

---

## Contributing

### Development Guidelines

1. **Follow Godot's API design patterns.**
2. **Minimize allocations** (use `ryml::cstring` instead of `std::string`).
3. **Error handling**: Use `YAMLResult` and throw `YAMLException` for C++ errors.
4. **Thread safety**: No global state; ensure **safe multithreading**.
5. **Write tests**: Every new feature should have **test coverage**.
6. **Document your changes**: All public APIs **must be documented**.

---

## License

MIT License (see [LICENSE](LICENSE) file for details).

---

**Built with ⚡ by [FimbulWorks](https://github.com/fimbul-works)**

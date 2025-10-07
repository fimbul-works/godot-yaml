# Godot YAML GDExtension

This is the **C++ GDExtension implementation** of the Godot YAML plugin. It provides **high-performance YAML parsing and serialization** using [RapidYAML](https://github.com/biojppm/rapidyaml) as the core engine, offering **sub-millisecond processing** for most YAML documents. This extension is built for Godot 4.3 or later.

## 🔄 Version History

- **2.1.0** (Current) - When passing custom tag to `YAML.schema_register(class, serialize_method, deserialize_static, custom_tag)` use the custom tag when emitting
- **2.0.0** - Major release with schema validation powered by [GDSchema](https://github.com/fimbul-works/gdschema), improved multi-document handling, and bug fixes. See [the full changelog](./project/addons/yaml/CHANGELOG.md#version-200) for details
- **1.1.0** - Nested YAML files can now be loaded with the `!Resource` tag, and some fixes
- **1.0.0** - First major release with custom YAML editor, streamlined API, and several fixes. See [the plugin changelog](project/addons/yaml/CHANGELOG.md#version-100) for details
- **0.12.1** - Build support for Linux (x86 64-bit)
- **0.12.0** - Performance optimizations, bug fixes, and comprehensive tests for all variant types
- **0.11.0** - Added support for parsing multiple documents, and error handling for custom class deserialization
- **0.10.1** - Fixed issue with custom Resources not being serializable
- **0.10.0** - Added custom class serialization support, upgraded to Godot 4.3
- **0.9.0** - Initial public release

## 🚀 Quick Start

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

## 🔥 Key Features

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

## 🛠️ Installation & Setup

### **Building From Source**

#### **Prerequisites**
- **Git** (for cloning and submodules)
- **Python 3.x** (for SCons build system)
- **C++ compiler** with C++17 support:
  - **Windows**: Visual Studio 2022 with C++ workload
  - **Linux/macOS**: GCC 9+ or Clang 10+
- **SCons** build system (`pip install scons`)

#### **Step 1: Clone the Repository**
```bash
# Clone with submodules
git clone --recursive https://github.com/fimbul-works/godot-yaml

# Or if already cloned, initialize submodules
git submodule update --init --recursive
```

#### **Step 2: Build the Extension**
```bash
# Debug build
scons target=template_debug

# Release build
scons target=template_release

# Specify platform (default is platform-dependent)
scons platform=windows target=template_release
scons platform=linux target=template_release
```

#### **Build Options**
- `platform`: Target (`windows`, `linux`, `macos`, etc.)
- `target`: Build type (`template_debug`, `template_release`)
- `arch`: CPU architecture (`x86_32`, `x86_64`, `arm64`, etc.)
- `dev_build`: Enable extra debugging (`yes`/`no`)
- `use_llvm`: Use Clang/LLVM compiler (`yes`/`no`)
- `verbose`: Verbose build output (`yes`/`no`)

---

## ✅ Supported Platforms

- **Windows**: ✅ Prebuilt binaries available for **x86 64-bit** architecture.
- **Linux**: ✅ Prebuilt binaries available for **x86 64-bit** architecture.
- **macOS**: ⚠️ Prebuilt binaries available for **universal** architecture.
  - **Note**: Some macOS configurations (particularly newer versions with stricter Gatekeeper policies) may prevent loading of GDExtensions generally, not just this plugin. If the extension fails to load, try building from source or test with other GDExtensions to determine if this is a system-wide issue.

---

## 🧑‍💻 **Contributing**

### **Development Guidelines**
1. **Follow Godot's API design patterns.**
2. **Minimize allocations** (use `ryml::cstring` instead of `std::string`).
3. **Error handling**: Use `YAMLResult` and throw `YAMLException` for C++ errors.
4. **Thread safety**: No global state; ensure **safe multithreading**.
5. **Write tests**: Every new feature should have **test coverage**.
6. **Document your changes**: All public APIs **must be documented**.

---

## 📜 **License**

MIT License (see [LICENSE](LICENSE) file for details).

---

🚀 **Built with ⚡ by [FimbulWorks](https://github.com/fimbul-works)**

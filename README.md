# Godot YAML GDExtension

This is the **C++ GDExtension implementation** of the Godot YAML plugin. It provides **high-performance YAML parsing and serialization** using [RapidYAML](https://github.com/biojppm/rapidyaml) as the core engine, offering **sub-millisecond processing** for most YAML documents. This extension is built for Godot 4.3 or later.

📌 **For full usage details and API documentation, see the plugin README:**
📂 [`project/addons/yaml/README.md`](project/addons/yaml/README.md)

## 🔄 Version History

- **0.10.0** (Current) - Added custom class serialization support, upgraded to Godot 4.3
- **0.9.0** - Initial public release

## 🚀 Quick Start

```gdscript
# Parse YAML
var result = YAML.parse("key: value\nlist:\n  - item1\n  - item2")
if !result.has_error():
    var data = result.get_data()
    print(data.key)  # Outputs: value
    print(data.list) # Outputs: [item1, item2]

# Generate YAML
var yaml = YAML.stringify({"numbers": [1, 2, 3]}).get_data()
print(yaml)  # Outputs: "numbers:\n  - 1\n  - 2\n  - 3\n"

# Register a custom class
class_name MyCustomClass extends RefCounted
var name = ""
var value = 0

static func from_dict(dict):
    var obj = MyCustomClass.new()
    obj.name = dict.get("name", "")
    obj.value = dict.get("value", 0)
    return obj

func to_dict():
    return {"name": name, "value": value}

# In your initialization code:
YAML.register_class(MyCustomClass)
```

## 🔥 Key Features

- ⚡ **High Performance** – Optimized for speed with zero-copy parsing.
- 🧩 **Full Variant Support** – Handles all\* **Godot built-in types**.
- 🎨 **Customizable Formatting** – Control YAML styles with `YAMLStyle`.
- 📌 **Tagged Types & Type Safety** – Support for custom YAML tags.
- 🔍 **Error Handling** – Detailed errors with line/column info.
- 🧵 **Thread-Safe** – No shared state, fully multi-threaded.
- 🛡️ **Validation** – Separate **lightweight syntax validation**.
- 🧪 **Custom Class Support** - Register GDScript classes for serialization/deserialization.

<sub>\* Except Callable, RID, or local Resources.</sub>

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

- **Windows**: ✅ Prebuilt binaries available.
- **Linux/macOS**: 🚧 Not yet prebuilt, but **should compile without issues**.

📌 **Contributions welcome!** If you can help with Linux/macOS, open a PR.

---

## ⚙️ **Error Handling in GDScript**

When working with YAML, errors **always return detailed messages** with line/column numbers.

#### **Example: Handling Parse Errors**
```gdscript
var result = YAML.parse("invalid_yaml: - missing_indent")

if result.has_error():
    print("❌ Error:", result.get_error_message())
    print("📍 Line:", result.get_error_line(), "Column:", result.get_error_column())
else:
    var data = result.get_data()
    print("✅ Parsed successfully:", data)
```

📂 **See [`project/addons/yaml/README.md`](project/addons/yaml/README.md) for more examples.**

---

## 🧑‍💻 **Contributing**

### **Development Guidelines**
1. **Follow Godot's API design patterns.**
2. **Minimize allocations** (use `ryml::cstring` instead of `std::string`).
3. **Error handling**: Use `YAMLResult` and throw `YAMLException` for C++ errors.
4. **Thread safety**: No global state; ensure **safe multithreading**.
5. **Write tests**: Every new feature should have **test coverage**.
6. **Document your changes**: All public APIs **must be documented**.

### **Code Formatting**
```bash
# Format code (requires clang-format)
clang-format -i src/*.cpp src/*.h src/variants/*.cpp src/variants/*.h
```

### **Adding New Type Converters**
1. **Create new type handler** in `variants/`.
2. **Inherit from `VariantConverter`**.
3. **Implement `encode` and `decode`** methods.
4. **Register in `converter_factory.cpp`**.
5. **Write test cases** in `variant_types.gd`.

---

## 🔮 **Future Roadmap**

- **Schema Validation** – Enforce YAML structure validation.

📌 **See [`project/addons/yaml/README.md`](project/addons/yaml/README.md) for updates.**

---

## 📜 **License**

MIT License (see [LICENSE](LICENSE) file for details).

---

🚀 **Built with ⚡ by [FimbulWorks](https://github.com/fimbul-works)**

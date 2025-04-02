#include "parser.h"
#include "class_registry.h"
#include "converter_factory.h"
#include "util_numeric.h"
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

YAML::Parser::Parser()
{
  // Setup ryml callbacks
  callbacks.m_error = error_callback;
  callbacks.m_user_data = this;

  // Initialize ryml components
  evt_handler = std::make_unique<ryml::EventHandlerTree>(callbacks);
  ryml_parser = std::make_unique<ryml::Parser>(evt_handler.get(), ryml::ParserOptions().locations(true));

  // Initialize converters
  init_converters();
}

void YAML::Parser::init_converters()
{
  // Get converters by type
  type_converters = factory.create_converter_set();

  // Build tag lookup table
  for (const auto& pair : type_converters) {
    if (pair.second) {
      tag_converters[pair.second->get_tag()] = pair.second.get();
    }
  }
}

void YAML::Parser::error_callback(const char* msg, size_t len, ryml::Location loc, void* user_data)
{
  ryml::csubstr error_msg(msg, len);

  // Strip "ERROR: " prefix if available
  const ryml::csubstr strip_error_prefix = "ERROR: ";
  if (error_msg.begins_with(strip_error_prefix)) {
    error_msg = error_msg.sub(strip_error_prefix.len);
  }

  // RapidYAML does not like complex keys
  if (error_msg.begins_with("ryml trees cannot handle containers as keys")) {
    error_msg = ryml::to_csubstr("unsupported complex key");
  }

  // Only return the first line, which has the relevent error message
  size_t newline_pos = error_msg.find('\n');
  if (newline_pos != ryml::substr::npos) {
    error_msg = error_msg.sub(0, newline_pos);
  }

  // Get the parser instance through user_data
  auto* parser = static_cast<Parser*>(user_data);
  if (!parser) {
    throw YAMLException(from_ryml_str(error_msg));
  }

  // Set error in this specific parser instance
  parser->current_result = YAMLResult::error(
          from_ryml_str(error_msg),
          loc.line,
          loc.col);

  // Error handler MUST throw!
  throw YAMLException(parser->current_result->get_error_message());
}

Ref<YAMLResult> YAML::Parser::parse(const String& input, bool p_detect_style)
{
  try {
    detect_style = p_detect_style;
    style = detect_style ? YAML::create_style() : nullptr;
    current_result = YAMLResult::success(Variant());
    tree.clear();
    current_path.clear();

    // Parse input into tree
    ryml::parse_in_arena(
            ryml_parser.get(),
            input.utf8().get_data(),
            &tree);

    if (tree.empty()) {
      return YAMLResult::error("Empty YAML document");
    }

    tree.resolve();

    // Disable style detection if multiple documents
    if (tree.rootref().is_stream() && tree.rootref().num_children() > 1) {
      detect_style = false;
    }

    if (detect_style) {
      detect_node_style(tree.rootref());
    }

    if (!current_result->has_error()) {
      if (tree.rootref().is_stream() && tree.rootref().num_children() > 1) {
        // Multi-document case
        Array documents;
        for (const auto& child : tree.rootref().children()) {
          documents.push_back(process_node(child));
        }
        current_result = YAMLResult::success(documents, style);
      } else {
        // Single document case
        Variant parsed_data = process_node(tree.rootref());
        current_result = YAMLResult::success(parsed_data, style);
      }
    }

    return current_result;

  } catch (const YAMLException& e) {
    return YAMLResult::error(e.what());
  } catch (const std::exception& e) {
    return YAMLResult::error(e.what());
  } catch (...) {
    return YAMLResult::error("Unknown error occurred during parsing");
  }
}

Variant YAML::Parser::process_node(const ryml::ConstNodeRef& node) const
{
  try {
    // First check for tagged values
    auto tagged = try_parse_tagged_value(node);
    if (tagged) {
      return *tagged;
    }

    // Handle different node types
    if (node.is_keyval()) {
      return process_value(node);
    } else if (node.is_map()) {
      return process_map(node);
    } else if (node.is_seq()) {
      return process_sequence(node);
    } else if (node.has_key()) {
      return process_key(node);
    } else if (node.has_val()) {
      return process_value(node);
    }

    return Variant();
  } catch (const std::exception& e) {
    ERR_PRINT(String("YAML parsing error: ") + e.what());
    return Variant();
  }
}

Variant YAML::Parser::process_map(const ryml::ConstNodeRef& node) const
{
  Dictionary dict;

  for (const auto& child : node.children()) {
    Variant key = process_key(child);
    if (key.get_type() == Variant::NIL && current_result->has_error()) {
      return Variant();
    }

    Variant value = process_node(child);
    if (value.get_type() == Variant::NIL && current_result->has_error()) {
      return Variant();
    }

    dict[key] = value;
  }

  return dict;
}

Variant YAML::Parser::process_sequence(const ryml::ConstNodeRef& node) const
{
  Array arr;

  for (const auto& child : node.children()) {
    Variant item = process_node(child);
    if (item.get_type() == Variant::NIL && current_result->has_error()) {
      return Variant();
    }
    arr.push_back(item);
  }

  return arr;
}

Variant YAML::Parser::process_key(const ryml::ConstNodeRef& node) const
{
  if (!node.has_key()) {
    return Variant();
  }
  return from_ryml_str(node.key());
}

Variant YAML::Parser::process_value(const ryml::ConstNodeRef& node) const
{
  // Handle null/empty values
  if (!node.has_val() || node.val().empty() || node.val_is_null()) {
    return Variant();
  }

  try {
    ryml::csubstr val = node.val();
    String str_val = from_ryml_str(val);

    // Handle special values first
    if (auto special_val = try_parse_special_value(str_val)) {
      return *special_val;
    }

    // Try numeric conversion
    if (auto num_val = try_parse_numeric_value(str_val, val)) {
      return *num_val;
    }

    // Return as string if no other type matches
    return str_val;
  } catch (const std::exception& e) {
    ERR_PRINT(String("Value parsing error: ") + e.what());
    return Variant();
  }
}

std::optional<Variant> YAML::Parser::try_parse_special_value(const String& str_val) const
{
  // Boolean values
  if (str_val == "true")
    return true;
  if (str_val == "false")
    return false;

  // Null values
  if (str_val == "null" || str_val == "~")
    return Variant();

  // Special float values
  if (str_val == ".inf" || str_val == "+.inf")
    return Math_INF;
  if (str_val == "-.inf")
    return -Math_INF;
  if (str_val == ".nan")
    return Math_NAN;

  return std::nullopt;
}

std::optional<Variant> YAML::Parser::try_parse_numeric_value(const String& str_val, const ryml::csubstr& val) const
{
  // Handle different integer formats
  if (str_val.begins_with("0b") || str_val.begins_with("0B") || // Binary
          str_val.begins_with("0o") || str_val.begins_with("0O") || // Octal
          str_val.begins_with("0x") || str_val.begins_with("0X") || // Hexadecimal
          (str_val.length() > 1 && str_val[0] == '0' && str_val[1] >= '0' && str_val[1] <= '7')) // Octal
  {
    try {
      return string_to_int<int64_t>(val);
    } catch (const std::exception& e) {
      return std::nullopt;
    }
  }

  // Handle decimal numbers
  if (str_val.is_valid_int()) {
    try {
      return string_to_int<int64_t>(val);
    } catch (const std::exception& e) {
      return std::nullopt;
    }
  }

  if (str_val.is_valid_float()) {
    try {
      return string_to_float<double>(val);
    } catch (const std::exception& e) {
      return std::nullopt;
    }
  }

  return std::nullopt;
}

std::optional<Variant> YAML::Parser::try_parse_tagged_value(const ryml::ConstNodeRef& node) const
{
  String tag = extract_tag(node);
  if (tag.is_empty()) {
    return std::nullopt;
  }

  // Read !!binary as a PackedByteArray
  if (tag == "!binary") {
    VariantConverter* converter = get_converter_for_type(Variant::PACKED_BYTE_ARRAY);
    if (converter) {
      return converter->decode(node);
    }
  }

  // Attempt to parse with a converter
  VariantConverter* converter = get_converter_for_tag(tag);
  if (converter) {
    return converter->decode(node);
  }

  // Check for custom converters
  if (YAMLClassRegistry::has_class(tag)) {
    // Only allow map-types
    if (!node.is_map()) {
      ERR_PRINT(vformat("Invalid node format for class %s - expected map", tag));
      return Variant();
    }
    YAMLClassRegistry::ClassInfo class_info = YAMLClassRegistry::get_class_info(tag);
    // Class info exists and is valid
    if (class_info.script_class.is_valid()) {
      Dictionary dict = process_map(node);
      return class_info.script_class->call(class_info.from_dict_method, dict);
    }
  }

  // Check if this is a registered class name
  if (ClassDB::class_exists(tag)) {
    return parse_object_or_resource(node, tag);
  }

  // For unknown tags, store the tag in style if available
  if (detect_style && style.is_valid()) {
    // Get the current path's style
    Ref<YAMLStyle> current_style = style;
    for (const auto& path_element : current_path) {
      current_style = current_style->get_child(String(path_element.c_str()));
      if (!current_style.is_valid()) {
        break;
      }
    }

    if (current_style.is_valid()) {
      // Store the tag in custom settings
      Dictionary custom_settings = current_style->get_custom_settings();
      custom_settings["tag"] = tag;
      current_style->set_custom_settings(custom_settings);
    }
  }

  // Return null to continue with normal processing in process_node
  return std::nullopt;
}

Variant YAML::Parser::parse_object_or_resource(const ryml::ConstNodeRef& node, const String& class_name) const
{
  // If the node contains just a string value, it might be a resource path
  if (node.has_val() && !node.is_map() && !node.is_seq()) {
    String path = from_ryml_str(node.val());
    if (path.begins_with("res://") || path.begins_with("user://")) {
      return load_resource(path);
    }
  }

  // Otherwise, treat it as an inline object/resource definition
  if (!node.is_map()) {
    ERR_PRINT(vformat("Invalid node format for class %s - expected map", class_name));
    return Variant();
  }

  // Instantiate the object
  Object* obj = ClassDB::instantiate(class_name);
  if (!obj) {
    ERR_PRINT(vformat("Failed to instantiate class: %s", class_name));
    return Variant();
  }

  // Process the object's properties
  bool success = populate_object_properties(obj, node);
  if (!success) {
    memdelete(obj);
    return Variant();
  }

  // Handle Resources vs regular Objects
  if (Object::cast_to<Resource>(obj)) {
    // For Resources, return a Ref<Resource>
    Ref<Resource> ref(Object::cast_to<Resource>(obj));
    return ref;
  } else {
    // For regular Objects, return the raw pointer
    // The owner of the returned Variant is responsible for cleanup
    return obj;
  }
}

Variant YAML::Parser::load_resource(const String& path) const
{
  ResourceLoader* loader = ResourceLoader::get_singleton();
  if (!loader) {
    ERR_PRINT("ResourceLoader singleton not available");
    return Variant();
  }

  Ref<Resource> resource = loader->load(path);
  if (!resource.is_valid()) {
    ERR_PRINT(vformat("Failed to load resource from path: %s", path));
    return Variant();
  }

  return resource;
}

bool YAML::Parser::populate_object_properties(Object* obj, const ryml::ConstNodeRef& node) const
{
  for (const auto& child : node.children()) {
    String key = from_ryml_str(child.key());
    Variant value = process_node(child);

    // Skip null values
    if (value.get_type() == Variant::NIL) {
      continue;
    }

    // Try to set the property
    UtilityFunctions::print("Set ", key, "=", value);
    obj->set(key, value);
  }

  return true;
}

String YAML::Parser::extract_tag(const ryml::ConstNodeRef& node) const
{
  if (!node.has_val_tag()) {
    return String();
  }

  const auto& tag = node.val_tag();
  if (tag.begins_with('!')) {
    return String::utf8(tag.sub(1).str, tag.len - 1);
  }
  return String::utf8(tag.str, tag.len);
}

VariantConverter* YAML::Parser::get_converter_for_type(Variant::Type type) const
{
  auto it = type_converters.find(type);
  return it != type_converters.end() ? it->second.get() : nullptr;
}

VariantConverter* YAML::Parser::get_converter_for_tag(const String& tag) const
{
  auto it = tag_converters.find(tag);
  return it != tag_converters.end() ? it->second : nullptr;
}

void YAML::Parser::detect_node_style(const ryml::ConstNodeRef& node)
{
  if (!detect_style || !style.is_valid()) {
    return;
  }

  // Get or create style for current path
  Ref<YAMLStyle> current_style;

  if (current_path.empty()) {
    current_style = style; // Use root style
  } else {
    // Navigate to current path
    current_style = style;
    for (const auto& path_element : current_path) {
      Ref<YAMLStyle> child = current_style->get_child(String(path_element.c_str()));
      if (!child.is_valid()) {
        child.instantiate();
        current_style->set_child(String(path_element.c_str()), child);
      }
      current_style = child;
    }
  }

  detect_node_style_internal(node, current_style);
}

void YAML::Parser::detect_node_style(const ryml::ConstNodeRef& node, const Ref<YAMLStyle>& target_style)
{
  if (!detect_style || !target_style.is_valid()) {
    return;
  }

  detect_node_style_internal(node, target_style);
}

void YAML::Parser::detect_node_style_internal(const ryml::ConstNodeRef& node, const Ref<YAMLStyle>& current_style)
{
  // Detect styles for this node
  detect_scalar_style(node, current_style);
  detect_container_form(node, current_style);
  detect_anchor_style(node, current_style);

  // Store custom tag if present
  String tag = extract_tag(node);
  if (!tag.is_empty()) {
    Dictionary custom_settings = current_style->get_custom_settings();
    custom_settings["tag"] = tag;
    current_style->set_custom_settings(custom_settings);
  }

  // For map nodes, process children with updated path
  if (node.is_map()) {
    for (const auto& child : node.children()) {
      std::string key(child.key().str, child.key().len);
      auto new_path = current_path;
      new_path.push_back(key);
      current_path = new_path;
      detect_node_style(child); // Use original method for path-based detection
      current_path.pop_back();
    }
  }
  // For sequence nodes, use indices as path elements
  else if (node.is_seq()) {
    // First detect template style if needed
    detect_array_template_style(node, current_path.empty() ? "_template" : current_path.back(), current_style);

    // Then process each element
    int index = 0;
    for (const auto& child : node.children()) {
      auto new_path = current_path;
      new_path.push_back(std::to_string(index++));
      current_path = new_path;
      detect_node_style(child); // Use original method for path-based detection
      current_path.pop_back();
    }
  }
}

void YAML::Parser::detect_scalar_style(const ryml::ConstNodeRef& node, const Ref<YAMLStyle>& style)
{
  if (!node.has_val()) {
    return;
  }

  // Set scalar style
  if (node.is_block()) {
    style->set_scalar_style(YAMLStyle::SCALAR_BLOCK);
  } else if (node.is_val_literal()) {
    style->set_scalar_style(YAMLStyle::SCALAR_LITERAL);
  } else if (node.is_val_folded()) {
    style->set_scalar_style(YAMLStyle::SCALAR_FOLDED);
  } else {
    style->set_scalar_style(YAMLStyle::SCALAR_PLAIN);
  }

  // Handle quoted styles
  if (node.is_val_quoted()) {
    style->set_quote_style(node.is_val_squo() ? YAMLStyle::QUOTE_SINGLE : YAMLStyle::QUOTE_DOUBLE);
  }

  // Detect number format
  String value = from_ryml_str(node.val());
  if (value.is_valid_float() || value.is_valid_int()) {
    if (value.begins_with("0x") || value.begins_with("0X")) {
      style->set_number_format(YAMLStyle::NUM_HEX);
    } else if (value.begins_with("0o") || value.begins_with("0O")) {
      style->set_number_format(YAMLStyle::NUM_OCTAL);
    } else if (value.begins_with("0b") || value.begins_with("0B")) {
      style->set_number_format(YAMLStyle::NUM_BINARY);
    } else if (value.find("e") != -1 || value.find("E") != -1) {
      style->set_number_format(YAMLStyle::NUM_SCIENTIFIC);
    } else {
      style->set_number_format(YAMLStyle::NUM_DECIMAL);
    }
  }
}

void YAML::Parser::detect_container_form(const ryml::ConstNodeRef& node, const Ref<YAMLStyle>& style)
{
  // Detect collection style
  if (node.is_seq()) {
    style->set_container_form(YAMLStyle::FORM_SEQ);
  } else if (node.is_map()) {
    style->set_container_form(YAMLStyle::FORM_MAP);
  } else {
    style->set_container_form(YAMLStyle::FORM_ANY);
  }

  // Detect flow stye
  if (node.is_flow()) {
    style->set_flow_style(YAMLStyle::FLOW_SINGLE);
  } else {
    style->set_flow_style(YAMLStyle::FLOW_NONE);
  }
}

void YAML::Parser::detect_anchor_style(const ryml::ConstNodeRef& node, const Ref<YAMLStyle>& style)
{
  Dictionary custom_settings;

  // Check for anchors and aliases
  if (node.has_anchor()) {
    custom_settings["anchor"] = from_ryml_str(node.val_anchor());
  }

  if (node.is_ref()) {
    custom_settings["alias"] = from_ryml_str(node.val_ref());
  }

  // Store custom settings if any were detected
  if (!custom_settings.is_empty()) {
    style->custom_settings = custom_settings;
  }
}

void YAML::Parser::detect_array_template_style(const ryml::ConstNodeRef& node, const std::string& key, Ref<YAMLStyle> current_style)
{
  if (!detect_style || !current_style.is_valid() || !node.is_seq() || node.num_children() == 0) {
    return;
  }

  // Create template style for the first element
  Ref<YAMLStyle> template_style;
  template_style.instantiate();

  // Save current path
  auto saved_path = current_path;

  // Clear path for template detection to avoid nesting
  current_path.clear();

  // Detect style from first element
  detect_node_style(node[0], template_style);

  // Store template in parent style
  current_style->set_child("_template", template_style);

  // Restore path
  current_path = saved_path;
}

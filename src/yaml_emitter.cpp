#include "yaml_emitter.h"
#include "util_numeric.h"
#include "yaml_result.h"

using namespace godot;

// Initialize static members
std::unordered_map<Variant::Type, String> YAMLEmitter::default_formats;
std::mutex YAMLEmitter::default_formats_mutex;

void YAMLEmitter::_bind_methods()
{
  ClassDB::bind_method(D_METHOD("emit", "input"), &YAMLEmitter::emit);
  ClassDB::bind_method(D_METHOD("set_format", "type", "format"), &YAMLEmitter::set_format);
  ClassDB::bind_method(D_METHOD("get_format", "type"), &YAMLEmitter::get_format);
  ClassDB::bind_method(D_METHOD("reset_formats"), &YAMLEmitter::reset_formats);

  ClassDB::bind_static_method("YAMLEmitter", D_METHOD("set_default_format", "type", "format"), &YAMLEmitter::set_default_format);
  ClassDB::bind_static_method("YAMLEmitter", D_METHOD("get_default_format", "type"), &YAMLEmitter::get_default_format);
  ClassDB::bind_static_method("YAMLEmitter", D_METHOD("reset_default_formats"), &YAMLEmitter::reset_default_formats);
}

YAMLEmitter::YAMLEmitter()
{
  // Copy current default formats to instance formats
  std::lock_guard<std::mutex> lock(default_formats_mutex);
  instance_formats = default_formats;
}

YAMLEmitter::~YAMLEmitter()
{
}

Ref<YAMLResult> YAMLEmitter::emit(const Variant& input)
{
  Ref<YAMLResult> result;
  result.instantiate();

  try {
    ryml::Tree tree;
    emit_recursively(tree.rootref(), input, result);

    if (result->has_error()) {
      return result;
    }

    if (tree.arena().empty()) {
      set_error(result, "Empty document");
      return result;
    }

    // Generate YAML string
    std::string str_result = ryml::emitrs_yaml<std::string>(tree);
    result->data = String::utf8(str_result.c_str(), str_result.length());
    return result;

  } catch (const std::exception& e) {
    set_error(result, String("Emission error: ") + e.what());
    return result;
  } catch (...) {
    set_error(result, "Unknown emission error");
    return result;
  }
}

void YAMLEmitter::emit_recursively(ryml::NodeRef& node, const Variant& v, Ref<YAMLResult>& result)
{
  switch (v.get_type()) {
    case Variant::NIL: {
      ryml::csubstr null = {};
      node << null;
      break;
    }
    case Variant::BOOL:
      node << ((bool)v ? "true" : "false");
      break;
    case Variant::INT:
    case Variant::FLOAT:
      emit_number(node, v);
      break;
    case Variant::STRING: {
      String str = v;
      emit_string(node, str);
      break;
    }
    case Variant::ARRAY:
      emit_array(node, v, result);
      break;
    case Variant::DICTIONARY:
      emit_map(node, v, result);
      break;
    default: {
      String type_name = Variant::get_type_name(v.get_type());
      set_error(result, vformat("Unsupported type: %s", type_name));
    }
  }
}

void YAMLEmitter::emit_map(ryml::NodeRef& node, const Dictionary& dict, Ref<YAMLResult>& result)
{
  node |= ryml::MAP;
  Array keys = dict.keys();

  for (int i = 0; i < keys.size(); ++i) {
    const Variant& key = keys[i];
    String key_str;

    // Convert key to string
    if (key.get_type() == Variant::STRING) {
      key_str = key;
    } else {
      key_str = String(key);
    }

    CharString key_utf8 = key_str.utf8();
    ryml::csubstr key_csubstr(key_utf8.get_data(), key_utf8.length());

    ryml::NodeRef child = node.append_child();
    child << ryml::key(key_csubstr);
    emit_recursively(child, dict[key], result);

    if (result->has_error()) {
      return;
    }
  }
}

void YAMLEmitter::emit_array(ryml::NodeRef& node, const Array& arr, Ref<YAMLResult>& result)
{
  node |= ryml::SEQ;

  for (int i = 0; i < arr.size(); ++i) {
    emit_recursively(node.append_child(), arr[i], result);
    if (result->has_error()) {
      return;
    }
  }
}

void YAMLEmitter::emit_string(ryml::NodeRef& node, const String& str)
{
  CharString utf8 = str.utf8();
  node << ryml::csubstr(utf8.get_data(), utf8.length());
}

void YAMLEmitter::emit_number(ryml::NodeRef& node, const Variant& v)
{
  if (v.get_type() == Variant::INT) {
    node << int_to_string((int64_t)v);
  } else {
    node << float_to_string((double)v);
  }
}

void YAMLEmitter::set_error(Ref<YAMLResult>& result, const String& error, int line, int column)
{
  result->set_error(error, line, column);
}

bool YAMLEmitter::set_format(Variant::Type type, const String& format)
{
  if (format.is_empty()) {
    instance_formats.erase(type);
  } else {
    instance_formats[type] = format;
  }
  return true;
}

String YAMLEmitter::get_format(Variant::Type type) const
{
  auto it = instance_formats.find(type);
  if (it != instance_formats.end()) {
    return it->second;
  }
  return String();
}

void YAMLEmitter::reset_formats()
{
  std::lock_guard<std::mutex> lock(default_formats_mutex);
  instance_formats = default_formats;
}

bool YAMLEmitter::set_default_format(Variant::Type type, const String& format)
{
  std::lock_guard<std::mutex> lock(default_formats_mutex);
  if (format.is_empty()) {
    default_formats.erase(type);
  } else {
    default_formats[type] = format;
  }
  return true;
}

String YAMLEmitter::get_default_format(Variant::Type type)
{
  std::lock_guard<std::mutex> lock(default_formats_mutex);
  auto it = default_formats.find(type);
  if (it != default_formats.end()) {
    return it->second;
  }
  return String();
}

void YAMLEmitter::reset_default_formats()
{
  std::lock_guard<std::mutex> lock(default_formats_mutex);
  default_formats.clear();
}

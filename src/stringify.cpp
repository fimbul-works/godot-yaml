#include "util_numeric.h"
#include "yaml.h"

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/resource.hpp>

using namespace godot;

String YAML::stringify(const Variant& input)
{
  std::string error_prefix = "YAML stringify error: ";
  error = "";
  try {
    ryml::Tree tree;
    emit_recursively(tree.rootref(), input);
    if (tree.arena().empty()) {
      UtilityFunctions::printerr(error_prefix.c_str(), "empty document");
      return Variant();
    }
    std::string str_result = ryml::emitrs_yaml<std::string>(tree);
    return String::utf8(str_result.c_str(), str_result.length());
  } catch (const YAMLException& e) {
    UtilityFunctions::printerr(error_prefix.c_str(), e.what());
    return Variant();
  } catch (const std::exception& e) {
    UtilityFunctions::printerr(error_prefix.c_str(), e.what());
    return Variant();
  } catch (...) {
    UtilityFunctions::printerr(error_prefix.c_str(), "unknown error");
    return Variant();
  }
}

void YAML::emit_recursively(ryml::NodeRef& node, const Variant& v)
{
  Variant::Type type = v.get_type();

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
      node << int_to_string((int64_t)v);
      break;
    case Variant::FLOAT:
      node << float_to_string((double)v);
      break;
    case Variant::STRING: {
      String godot_str = v;
      node << godot_str.utf8();
      break;
    }
    case Variant::ARRAY: {
      node |= ryml::SEQ;
      Array arr = v;
      for (int i = 0; i < arr.size(); ++i) {
        emit_recursively(node.append_child(), arr[i]);
      }
      break;
    }
    case Variant::DICTIONARY: {
      node |= ryml::MAP;
      Dictionary dict = v;
      Array keys = dict.keys();
      for (int i = 0; i < keys.size(); ++i) {
        String key = keys[i];
        CharString key_utf8 = key.utf8();
        ryml::csubstr key_csubstr(key_utf8.get_data(), key_utf8.length());
        ryml::NodeRef child = node.append_child();
        child << ryml::key(key_csubstr);
        emit_recursively(child, dict[key]);
      }
      break;
    }
    case Variant::CALLABLE:
      UtilityFunctions::push_warning("YAML warning: Callable cannot be stringified");
      break;
    case Variant::OBJECT: {
      if (Object::cast_to<Resource>(v)) {
        node.set_val_tag(ryml::to_csubstr(resource_encoder->get_tag()));
        resource_encoder->encode(node, v);
        break;
      }
      UtilityFunctions::push_warning("YAML warning: Object cannot be stringified");
      break;
    }
    default:
      auto it = type_to_encoder.find(type);
      if (it != type_to_encoder.end()) {
        const char* full_tag = it->second->get_full_tag();
        if (full_tag == nullptr || full_tag[0] == '\0') {
          UtilityFunctions::printerr("Error: Empty or null tag for type ", Variant::get_type_name(type));
          return;
        }

        node.set_val_tag(ryml::to_csubstr(full_tag));

        try {
          it->second->encode(node, v);
        } catch (const std::exception& e) {
          UtilityFunctions::printerr("Error encoding type ", Variant::get_type_name(type), ": ", e.what());
        }
        return;
      }
      // auto it = type_to_encoder.find(type);
      // if (it != type_to_encoder.end()) {
      //   node.set_val_tag(ryml::to_csubstr(it->second->get_tag()));
      //   it->second->encode(node, v);
      //   return;
      // }
      throw YAMLException("Unsupported type " + v.get_type_name(v.get_type()));
  }
}

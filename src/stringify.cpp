#include "yaml.h"

// #include "ryml_extras.h"
#include "util_emit.h"

using namespace godot;

String YAML::stringify(const Variant& input)
{
  ryml::Tree tree;
  emit_recursively(tree.rootref(), input);
  std::string str_result = ryml::emitrs_yaml<std::string>(tree);
  return String::utf8(str_result.c_str(), str_result.length());
}

void YAML::emit_recursively(ryml::NodeRef& node, const Variant& v)
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
      node << (int64_t)v;
      break;

    case Variant::FLOAT:
      emit_float(node, (double)v);
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

    default:
      UtilityFunctions::printerr("Unsupported type: ", v.get_type_name(v.get_type()));
      break;
  }
}

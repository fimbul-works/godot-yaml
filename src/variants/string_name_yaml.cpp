#include "string_name_yaml.h"
#include "yaml.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

StringNameYAMLEncoder::StringNameYAMLEncoder(YAML* yaml) :
        YAMLEncoder(yaml) { }

void StringNameYAMLEncoder::encode(ryml::NodeRef& node, const Variant& v) const
{
  StringName str_name = v.operator StringName();
  node << String(str_name).utf8().get_data();
}

Variant StringNameYAMLEncoder::decode(const ryml::ConstNodeRef& node) const
{
  if (node.has_val() && !node.val_is_null()) {
    return StringName(String::utf8(node.val().str, node.val().len));
  }
  throw YAMLException("invalid StringName format - " + String::utf8(node.val().str, node.val().len));
}

bool StringNameYAMLEncoder::set_format(const String& format_str)
{
  return true;
}

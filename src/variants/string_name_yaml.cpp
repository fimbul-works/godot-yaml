#include "string_name_yaml.h"
#include "../yaml_exception.h"

using namespace godot;

StringNameVariantConverter::StringNameVariantConverter(YAML* yaml) :
        VariantConverter(yaml) { }

void StringNameVariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  StringName str_name = v.operator StringName();

  // Convert to string first to handle empty StringName properly
  String str = String(str_name);
  if (str.is_empty()) {
    // Empty StringName is represented as null
    ryml::csubstr null = {};
    node << null;
  } else {
    node << str.utf8().get_data();
  }
}

Variant StringNameVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (node.val_is_null()) {
    return StringName(); // Return empty StringName
  }

  if (!node.has_val()) {
    throw YAMLException::create_invalid_format("StringName");
  }

  try {
    String str = String::utf8(node.val().str, node.val().len);
    return StringName(str);
  } catch (const std::exception& e) {
    throw YAMLException(String("Failed to decode StringName: ") + e.what());
  }
}

bool StringNameVariantConverter::set_format(const String& format_str)
{
  // StringName only supports a single string format
  return true;
}

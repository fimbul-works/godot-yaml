#include "string_name_yaml.h"
#include "../yaml_exception.h"

using namespace godot;

void StringNameVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const
{
  const StringName str = v.operator StringName();
  emit_as_string(node, str);
}

void StringNameVariantConverter::emit_as_string(ryml::NodeRef& node, const StringName& str) const
{
  String string_val = String(str);
  if (string_val.is_empty()) {
    // Empty StringName is represented as null
    ryml::csubstr null = {};
    node << null;
  } else {
    node << string_val.utf8().get_data();
  }
}

Variant StringNameVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  try {
    if (node.val_is_null()) {
      return StringName(); // Return empty StringName
    }

    if (!node.has_val()) {
      throw YAMLException::create_invalid_format("StringName");
    }

    return decode_from_string(node.val());
  } catch (const YAMLException&) {
    throw;
  } catch (const std::exception& e) {
    throw YAMLException::create_decode_error("StringName", e.what());
  }
}

Variant StringNameVariantConverter::decode_from_string(const ryml::csubstr& val) const
{
  try {
    String string_val = String::utf8(val.str, val.len);
    return StringName(string_val);
  } catch (const std::exception& e) {
    throw YAMLException(String("Invalid StringName format: ") + e.what());
  }
}

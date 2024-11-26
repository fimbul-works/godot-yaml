#include "format.h"
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

// Static member initialization
std::once_flag YAMLFormat::s_init_flag;
const std::unordered_map<YAMLFormat::Format, const char*> YAMLFormat::s_format_names = {
  { DEFAULT, "DEFAULT" },
  { FLOW_MAP, "FLOW_MAP" },
  { BLOCK_MAP, "BLOCK_MAP" },
  { SEQUENCE, "SEQUENCE" },
  { INLINE, "INLINE" },
  { HEX, "HEX" },
  { HEX_STRING, "HEX_STRING" },
  { BASE64, "BASE64" },
  { CUSTOM_1, "CUSTOM_1" },
  { CUSTOM_2, "CUSTOM_2" },
  { CUSTOM_3, "CUSTOM_3" }
};

YAMLFormat::YAMLFormat()
{
  // Empty constructor, formats start empty
}

YAMLFormat::~YAMLFormat()
{
  // Cleanup handled by std::unordered_map
}

Ref<YAMLFormat> YAMLFormat::create_default()
{
  Ref<YAMLFormat> format;
  format.instantiate();

  // Set default formats if needed
  // Example: format->set_format(Variant::PACKED_BYTE_ARRAY, BASE64);

  return format;
}

Error YAMLFormat::set_format(Variant::Type type, Format format)
{
  ERR_FAIL_COND_V_MSG(!is_valid_format(format),
          ERR_INVALID_PARAMETER,
          vformat("Invalid format value: %d", format));

  ERR_FAIL_COND_V_MSG(type < 0 || type >= Variant::VARIANT_MAX,
          ERR_INVALID_PARAMETER,
          vformat("Invalid variant type: %d", type));

  formats[type] = format;
  return OK;
}

YAMLFormat::Format YAMLFormat::get_format(Variant::Type type) const
{
  auto it = formats.find(type);
  return it != formats.end() ? static_cast<Format>(it->second) : DEFAULT;
}

void YAMLFormat::clear()
{
  formats.clear();
}

String YAMLFormat::get_format_name(Format format)
{
  if (!is_valid_format(format)) {
    return "INVALID_FORMAT";
  }

  auto it = s_format_names.find(format);
  return it != s_format_names.end() ? String(it->second) : "UNKNOWN";
}

Ref<YAMLFormat> YAMLFormat::duplicate() const
{
  Ref<YAMLFormat> new_format;
  new_format.instantiate();
  new_format->formats = formats;
  return new_format;
}

void YAMLFormat::_bind_methods()
{
  // Instance methods
  ClassDB::bind_method(D_METHOD("set_format", "type", "format"), &YAMLFormat::set_format);
  ClassDB::bind_method(D_METHOD("get_format", "type"), &YAMLFormat::get_format);
  ClassDB::bind_method(D_METHOD("clear"), &YAMLFormat::clear);
  ClassDB::bind_method(D_METHOD("duplicate"), &YAMLFormat::duplicate);

  // Static methods
  ClassDB::bind_static_method("YAMLFormat", D_METHOD("create_default"), &YAMLFormat::create_default);
  ClassDB::bind_static_method("YAMLFormat", D_METHOD("get_format_name", "format"), &YAMLFormat::get_format_name);
  ClassDB::bind_static_method("YAMLFormat", D_METHOD("is_valid_format", "format"), &YAMLFormat::is_valid_format);

  // Enum bindings
  BIND_ENUM_CONSTANT(DEFAULT);
  BIND_ENUM_CONSTANT(FLOW_MAP);
  BIND_ENUM_CONSTANT(BLOCK_MAP);
  BIND_ENUM_CONSTANT(SEQUENCE);
  BIND_ENUM_CONSTANT(INLINE);
  BIND_ENUM_CONSTANT(HEX);
  BIND_ENUM_CONSTANT(HEX_STRING);
  BIND_ENUM_CONSTANT(BASE64);
  BIND_ENUM_CONSTANT(CUSTOM_1);
  BIND_ENUM_CONSTANT(CUSTOM_2);
  BIND_ENUM_CONSTANT(CUSTOM_3);
  BIND_ENUM_CONSTANT(FORMAT_MAX);
}

} // namespace godot

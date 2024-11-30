#include "format.h"
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

// Static member initialization
std::once_flag YAMLFormat::s_init_flag;

const std::unordered_map<YAMLFormat::Format, const char*> YAMLFormat::s_format_names = {
  { DEFAULT, "DEFAULT" },
  { MAP, "MAP" },
  { SEQUENCE, "SEQUENCE" },
  { FLOW_MAP, "FLOW_MAP" },
  { FLOW_SEQUENCE, "FLOW_SEQUENCE" },
  { HEX, "HEX" },
  { HEX_STRING, "HEX_STRING" },
  { BASE64, "BASE64" },
};

void YAMLFormat::_bind_methods()
{
  // Instance methods
  ClassDB::bind_method(D_METHOD("set_format", "type", "format"), &YAMLFormat::set_format);
  ClassDB::bind_method(D_METHOD("get_format", "type"), &YAMLFormat::get_format);

  // Static methods
  ClassDB::bind_static_method("YAMLFormat", D_METHOD("is_valid_format", "format"), &YAMLFormat::is_valid_format);
  ClassDB::bind_static_method("YAMLFormat", D_METHOD("get_format_name", "format"), &YAMLFormat::get_format_name);

  // Enum bindings
  BIND_ENUM_CONSTANT(DEFAULT);
  BIND_ENUM_CONSTANT(MAP);
  BIND_ENUM_CONSTANT(SEQUENCE);
  BIND_ENUM_CONSTANT(FLOW_MAP);
  BIND_ENUM_CONSTANT(FLOW_SEQUENCE);
  BIND_ENUM_CONSTANT(HEX);
  BIND_ENUM_CONSTANT(HEX_STRING);
  BIND_ENUM_CONSTANT(BASE64);
  BIND_ENUM_CONSTANT(FORMAT_MAX);
}

Error YAMLFormat::set_format(Variant::Type type, Format format)
{
  ERR_FAIL_COND_V_MSG(type < 0 || type >= Variant::VARIANT_MAX,
          ERR_INVALID_PARAMETER,
          vformat("Invalid variant type: %d", type));

  ERR_FAIL_COND_V_MSG(!is_valid_format(format),
          ERR_INVALID_PARAMETER,
          vformat("Invalid format value: %d", format));

  formats[type] = format;
  return OK;
}

YAMLFormat::Format YAMLFormat::get_format(Variant::Type type) const
{
  auto it = formats.find(type);
  return it != formats.end() ? static_cast<Format>(it->second) : DEFAULT;
}

String YAMLFormat::get_format_name(Format format)
{
  if (!is_valid_format(format)) {
    return "INVALID_FORMAT";
  }

  auto it = s_format_names.find(format);
  return it != s_format_names.end() ? String(it->second) : "UNKNOWN";
}

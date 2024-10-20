#include "packed_byte_array_yaml.h"
#include "yaml.h"

#include <godot_cpp/classes/marshalls.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <iomanip>
#include <sstream>

using namespace godot;

PackedByteArrayYAMLEncoder::PackedByteArrayYAMLEncoder(YAML* yaml) :
        YAMLEncoder(yaml) { }

void PackedByteArrayYAMLEncoder::encode(ryml::NodeRef& node, const Variant& v) const
{
  PackedByteArray array = v.operator PackedByteArray();
  switch (format) {
    case Format::HEX:
      emit_as_hex(node, array);
      break;
    case Format::BASE64:
      emit_as_base64(node, array);
      break;
  }
}

Variant PackedByteArrayYAMLEncoder::decode(const ryml::ConstNodeRef& node) const
{
  if (node.has_val() && !node.val_is_null()) {
    String data = String::utf8(node.val().str, node.val().len);
    if (is_hex(data)) {
      PackedByteArray array = PackedByteArray();
      int size = data.length() / 2;
      array.resize(size);
      for (int i = 0; i < size; ++i) {
        String byte_str = data.substr(i * 2, 2);
        array[i] = static_cast<uint8_t>(byte_str.hex_to_int());
      }
      return array;
    } else {
      return Marshalls::get_singleton()->base64_to_raw(data);
    }
  }
  throw YAMLException("invalid PackedByteArray format - " + String::utf8(node.val().str, node.val().len));
}

bool PackedByteArrayYAMLEncoder::set_format(const String& format_str)
{
  if (format_str == "hex") {
    format = Format::HEX;
  } else if (format_str == "base64") {
    format = Format::BASE64;
  }
  return true;
}

void PackedByteArrayYAMLEncoder::emit_as_hex(ryml::NodeRef& node, const PackedByteArray& array) const
{
  std::stringstream ss;
  for (int i = 0; i < array.size(); ++i) {
    ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(array[i]);
  }
  node << ss.str();
}

void PackedByteArrayYAMLEncoder::emit_as_base64(ryml::NodeRef& node, const PackedByteArray& array) const
{
  String base64 = Marshalls::get_singleton()->raw_to_base64(array);
  node << ryml::csubstr(base64.utf8().get_data());
}

bool PackedByteArrayYAMLEncoder::is_hex(const String& s) const
{
  if (s.length() % 2 != 0) {
    return false;
  }
  for (int i = 0; i < s.length(); ++i) {
    char32_t c = s[i];
    if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
      return false;
    }
  }
  return true;
}

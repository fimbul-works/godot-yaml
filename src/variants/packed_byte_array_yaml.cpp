#include "packed_byte_array_yaml.h"
#include "../yaml_exception.h"
#include <godot_cpp/classes/marshalls.hpp>
#include <iomanip>
#include <sstream>

using namespace godot;

PackedByteArrayVariantConverter::PackedByteArrayVariantConverter(YAML* yaml) :
        VariantConverter(yaml) { }

void PackedByteArrayVariantConverter::encode(ryml::NodeRef& node, const Variant& v) const
{
  PackedByteArray array = v.operator PackedByteArray();

  if (array.size() == 0) {
    // Empty array is represented as null
    ryml::csubstr null = {};
    node << null;
    return;
  }

  switch (format) {
    case Format::HEX:
      emit_as_hex(node, array);
      break;
    case Format::BASE64:
      emit_as_base64(node, array);
      break;
  }
}

Variant PackedByteArrayVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (node.val_is_null()) {
    return PackedByteArray(); // Return empty array
  }

  if (!node.has_val()) {
    throw YAMLException::create_invalid_format("PackedByteArray");
  }

  String data = String::utf8(node.val().str, node.val().len);

  try {
    if (is_hex(data)) {
      return hex_to_bytes(data);
    } else {
      // Try base64 decode if not hex
      return Marshalls::get_singleton()->base64_to_raw(data);
    }
  } catch (const std::exception& e) {
    throw YAMLException(String("Failed to decode PackedByteArray: ") + e.what());
  }
}

bool PackedByteArrayVariantConverter::set_format(const String& format_str)
{
  if (format_str == "hex") {
    format = Format::HEX;
    return true;
  } else if (format_str == "base64") {
    format = Format::BASE64;
    return true;
  }
  return false;
}

void PackedByteArrayVariantConverter::emit_as_hex(ryml::NodeRef& node, const PackedByteArray& array) const
{
  std::stringstream ss;
  ss << std::hex << std::setfill('0');

  for (int i = 0; i < array.size(); ++i) {
    ss << std::setw(2) << static_cast<int>(array[i]);
  }

  node << ss.str();
}

void PackedByteArrayVariantConverter::emit_as_base64(ryml::NodeRef& node, const PackedByteArray& array) const
{
  String base64 = Marshalls::get_singleton()->raw_to_base64(array);
  node << base64.utf8().get_data();
}

bool PackedByteArrayVariantConverter::is_hex(const String& s) const
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

PackedByteArray PackedByteArrayVariantConverter::hex_to_bytes(const String& hex) const
{
  if (!is_hex(hex)) {
    throw YAMLException("Invalid hex string format");
  }

  PackedByteArray array;
  int size = hex.length() / 2;
  array.resize(size);

  for (int i = 0; i < size; ++i) {
    String byte_str = hex.substr(i * 2, 2);
    array.set(i, static_cast<uint8_t>(byte_str.hex_to_int()));
  }

  return array;
}

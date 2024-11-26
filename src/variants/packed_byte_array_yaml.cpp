#include "packed_byte_array_yaml.h"
#include "../exception.h"
#include "../util_string.h"

#include <godot_cpp/classes/marshalls.hpp>

using namespace godot;

void PackedByteArrayVariantConverter::encode(ryml::NodeRef& node, const Variant& v, const YAMLFormat::View& format) const
{
  const PackedByteArray array = v.operator PackedByteArray();

  if (array.size() == 0) {
    ryml::csubstr null = {};
    node << null;
    return;
  }

  switch (format.get_format(Variant::PACKED_BYTE_ARRAY)) {
    case YAMLFormat::HEX:
      emit_as_hex(node, array);
      break;
    case YAMLFormat::BASE64:
    default:
      emit_as_base64(node, array);
      break;
  }
}

void PackedByteArrayVariantConverter::emit_as_hex(ryml::NodeRef& node, const PackedByteArray& array) const
{
  std::vector<char> hex_str;
  hex_str.reserve(array.size() * 2);

  static const char hex_chars[] = "0123456789ABCDEF";
  for (int i = 0; i < array.size(); ++i) {
    uint8_t byte = array[i];
    hex_str.push_back(hex_chars[byte >> 4]);
    hex_str.push_back(hex_chars[byte & 0xF]);
  }

  node << format_output(ryml::csubstr(hex_str.data(), hex_str.size()), HEX_LINE_LENGTH);
}

void PackedByteArrayVariantConverter::emit_as_base64(ryml::NodeRef& node, const PackedByteArray& array) const
{
  String base64 = Marshalls::get_singleton()->raw_to_base64(array);
  node << format_output(to_ryml_str(base64), BASE64_LINE_LENGTH);
}

Variant PackedByteArrayVariantConverter::decode(const ryml::ConstNodeRef& node) const
{
  if (node.val_is_null()) {
    return PackedByteArray();
  }

  if (!node.has_val()) {
    throw YAMLException::create_invalid_format("PackedByteArray");
  }

  try {
    CleanupResult result = cleanup_and_detect(node.val());

    if (result.is_hex) {
      return hex_to_bytes(result.cleaned);
    } else {
      return Marshalls::get_singleton()->base64_to_raw(String::utf8(result.cleaned.c_str()));
    }
  } catch (const std::exception& e) {
    throw YAMLException::create_decode_error("PackedByteArray", e.what());
  }
}

PackedByteArrayVariantConverter::CleanupResult
PackedByteArrayVariantConverter::cleanup_and_detect(const ryml::csubstr& input) const
{
  std::string cleaned;
  cleaned.reserve(input.len);
  bool is_hex = true;

  for (size_t i = 0; i < input.len; i++) {
    char c = input.str[i];
    if (!is_whitespace(c)) {
      if (is_hex && !is_hex_char(c)) {
        is_hex = false;
      }
      cleaned += c;
    }
  }

  // Validate hex string length
  if (is_hex && cleaned.length() % 2 != 0) {
    throw YAMLException("Invalid hex string length - must be even");
  }

  return { std::move(cleaned), is_hex, input.len };
}

// In the format_output method, change:
ryml::csubstr PackedByteArrayVariantConverter::format_output(ryml::csubstr str, size_t line_length) const
{
  if (str.len <= line_length) {
    return str;
  }

  // Create the output buffer directly as a char array
  size_t num_lines = (str.len + line_length - 1) / line_length;
  size_t total_size = str.len + (num_lines - 1);

  std::vector<char> formatted(total_size);
  size_t pos = 0;
  size_t out_pos = 0;

  while (pos < str.len) {
    if (pos > 0) {
      formatted[out_pos++] = '\n';
    }
    size_t chunk_size = std::min(line_length, str.len - pos);
    memcpy(&formatted[out_pos], str.str + pos, chunk_size);
    out_pos += chunk_size;
    pos += chunk_size;
  }

  // Return a view into the buffer
  return ryml::csubstr(formatted.data(), out_pos);
}

PackedByteArray PackedByteArrayVariantConverter::hex_to_bytes(const std::string& hex) const
{
  PackedByteArray array;
  array.resize(hex.length() / 2);

  for (size_t i = 0; i < array.size(); ++i) {
    unsigned int byte;
    std::sscanf(hex.c_str() + i * 2, "%2x", &byte);
    array.set(i, static_cast<uint8_t>(byte));
  }

  return array;
}

bool PackedByteArrayVariantConverter::is_hex_char(char c)
{
  return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

bool PackedByteArrayVariantConverter::is_whitespace(char c)
{
  return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

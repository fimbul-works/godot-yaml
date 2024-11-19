#ifndef PACKED_BYTE_ARRAY_YAML_H
#define PACKED_BYTE_ARRAY_YAML_H

#include "../variant_converter.h"

namespace godot {

class PackedByteArrayVariantConverter : public VariantConverter {
  enum class Format {
    HEX, // Continuous hex string
    BASE64 // Base64 encoded string
  };

  public:
  DEFINE_YAML_TAG("PackedByteArray", Variant::PACKED_BYTE_ARRAY)

  PackedByteArrayVariantConverter(YAML* yaml);

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;
  bool set_format(const String& format) override;

  private:
  Format format = Format::HEX;

  void emit_as_hex(ryml::NodeRef& node, const PackedByteArray& array) const;
  void emit_as_base64(ryml::NodeRef& node, const PackedByteArray& array) const;
  bool is_hex(const String& s) const;
  PackedByteArray hex_to_bytes(const String& hex) const;
};

} // namespace godot

#endif // PACKED_BYTE_ARRAY_YAML_H

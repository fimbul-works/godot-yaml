#ifndef PACKED_BYTE_ARRAY_YAML_H
#define PACKED_BYTE_ARRAY_YAML_H

#include "yaml_encoder.h"

namespace godot {

class PackedByteArrayYAMLEncoder : public IYAMLEncoder {
  enum class Format {
    HEX,
    BASE64
  };

  public:
  const char* get_tag() const override;

  void encode(ryml::NodeRef& node, const Variant& v) const override;
  Variant decode(const ryml::ConstNodeRef& node) const override;

  bool set_format(const String& format_str) override;

  private:
  void emit_as_hex(ryml::NodeRef& node, const PackedByteArray& array) const;
  void emit_as_base64(ryml::NodeRef& node, const PackedByteArray& array) const;

  bool is_hex(const String& s) const;

  Format format = Format::HEX;
};

} // namespace godot

#endif // PACKED_BYTE_ARRAY_YAML_H

#ifndef YAML_FORMAT_H
#define YAML_FORMAT_H

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/variant.hpp>

#include <unordered_map>

namespace godot {

class YAMLFormat : public RefCounted {
  GDCLASS(YAMLFormat, RefCounted);

  protected:
  static void _bind_methods();

  public:
  // Format types available to all variant converters
  enum Format {
    // Default formats
    DEFAULT = 0,
    MAP = 1,
    SEQUENCE = 2,
    FLOW_MAP = 3,
    FLOW_SEQUENCE = 4,

    // Special formats
    HEX = 5,
    HEX_STRING = 6,
    BASE64 = 7,

    // Add new formats above this line
    FORMAT_MAX = 8
  };

  YAMLFormat() = default;

  // Format management
  Error set_format(Variant::Type type, Format format);
  Format get_format(Variant::Type type) const;

  // Debug helpers
  static String get_format_name(Format format);
  static bool is_valid_format(Format format) { return format >= DEFAULT && format < FORMAT_MAX; }

  // Internal view for encoders (not exposed to GDScript)
  class View {
    friend class YAMLFormat;

public:
    Format get_format(Variant::Type type) const
    {
      auto it = formats.find(type);
      return it != formats.end() ? static_cast<Format>(it->second) : DEFAULT;
    }

private:
    View(const std::unordered_map<Variant::Type, int>& f) :
            formats(f) { }
    const std::unordered_map<Variant::Type, int>& formats;
  };

  View get_view() const { return View(formats); }

  private:
  std::unordered_map<Variant::Type, int> formats;
  static const std::unordered_map<Format, const char*> s_format_names;
  static void initialize_format_names();
  static std::once_flag s_init_flag;
};

} // namespace godot

VARIANT_ENUM_CAST(YAMLFormat::Format);

#endif // YAML_FORMAT_H

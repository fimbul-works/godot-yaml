#ifndef YAML_RESULT_H
#define YAML_RESULT_H

#include "style.h"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/variant.hpp>

namespace godot {

// Forward reference
class YAMLStyle;

class YAMLResult : public RefCounted {
  GDCLASS(YAMLResult, RefCounted)

  protected:
  static void _bind_methods();

  public:
  // Default constructor that creates an empty successful result
  YAMLResult() :
          data(Variant()), error_message(""), error_line(-1), error_column(-1) { }

  // Static factory methods
  static Ref<YAMLResult> success(const Variant& data, const Ref<YAMLStyle>& style = nullptr);
  static Ref<YAMLResult> error(const String& msg, int line = -1, int column = -1);

  // Immutable accessors
  Variant get_data(int index = 0) const;
  Variant get_document(int index = 0) const { return get_data(index); }
  int get_document_count() const;
  bool has_error() const { return !error_message.is_empty(); }
  String get_error_message() const { return error_message; }
  int get_error_line() const { return error_line; }
  int get_error_column() const { return error_column; }

  // Style handling
  bool has_style() const { return style.is_valid(); }
  Ref<YAMLStyle> get_style() const { return style; }

  private:
  // Private constructor to enforce factory method usage
  YAMLResult(
          const Variant& data_,
          const Ref<YAMLStyle>& style_ = nullptr,
          const String& error_ = "",
          int line = -1,
          int col = -1) :
          data(data_), style(style_), error_message(error_), error_line(line), error_column(col) { }

  // Immutable state
  const Variant data;
  const String error_message;
  const int error_line;
  const int error_column;
  const Ref<YAMLStyle> style;
};

} // namespace godot

#endif // YAML_RESULT_H

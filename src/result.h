// yaml_result.h
#ifndef YAML_RESULT_H
#define YAML_RESULT_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/variant.hpp>

namespace godot {

class YAMLResult : public RefCounted {
  GDCLASS(YAMLResult, RefCounted)

  protected:
  static void _bind_methods();

  public:
  // Default constructor that creates an empty successful result
  YAMLResult() :
          data(Variant()), error_message(""), error_line(-1), error_column(-1) { }

  // Static factory methods
  static Ref<YAMLResult> success(const Variant& data);
  static Ref<YAMLResult> error(const String& msg, int line = -1, int column = -1);

  // Immutable accessors
  Variant get_data() const { return data; }
  bool has_error() const { return !error_message.is_empty(); }
  String get_error() const;
  int get_error_line() const { return error_line; }
  int get_error_column() const { return error_column; }

  private:
  // Private constructor to enforce factory method usage
  YAMLResult(const Variant& data_, const String& error_ = "", int line = -1, int col = -1) :
          data(data_), error_message(error_), error_line(line), error_column(col)
  {
  }

  // Immutable state
  const Variant data;
  const String error_message;
  const int error_line;
  const int error_column;
};

} // namespace godot

#endif // YAML_RESULT_H

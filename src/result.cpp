#include "result.h"
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

void YAMLResult::_bind_methods()
{
  ClassDB::bind_method(D_METHOD("has_error"), &YAMLResult::has_error);
  ClassDB::bind_method(D_METHOD("get_error"), &YAMLResult::get_error);
  ClassDB::bind_method(D_METHOD("get_data"), &YAMLResult::get_data);
  ClassDB::bind_method(D_METHOD("get_error_line"), &YAMLResult::get_error_line);
  ClassDB::bind_method(D_METHOD("get_error_column"), &YAMLResult::get_error_column);
}

YAMLResult::YAMLResult(const Variant& data_, const String& error_, int line, int col) :
        data(data_), error_message(error_), error_line(line), error_column(col)
{
}

Ref<YAMLResult> YAMLResult::success(const Variant& data)
{
  return Ref<YAMLResult>(memnew(YAMLResult(data)));
}

Ref<YAMLResult> YAMLResult::error(const String& msg, int line, int column)
{
  return Ref<YAMLResult>(memnew(YAMLResult(Variant(), msg, line, column)));
}

String YAMLResult::get_error() const
{
  if (error_line >= 0 && error_column >= 0) {
    return vformat("%s at line %d, column %d", error_message, error_line, error_column);
  }
  return error_message;
}

} // namespace godot

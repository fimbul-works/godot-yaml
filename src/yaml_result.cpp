#include "yaml_result.h"

using namespace godot;

void YAMLResult::_bind_methods()
{
  ClassDB::bind_method(D_METHOD("has_error"), &YAMLResult::has_error);
  ClassDB::bind_method(D_METHOD("get_error"), &YAMLResult::get_error);
  ClassDB::bind_method(D_METHOD("get_data"), &YAMLResult::get_data);
}

YAMLResult::YAMLResult() :
        error_line(-1), error_column(-1) { }

void YAMLResult::reset()
{
  std::lock_guard<std::mutex> lock(mutex);
  error_message = "";
  error_line = -1;
  error_column = -1;
  data = Variant();
}

void YAMLResult::set_error(const String& msg, int line, int column)
{
  std::lock_guard<std::mutex> lock(mutex);
  error_message = msg;
  error_line = line;
  error_column = column;
}

bool YAMLResult::has_error() const
{
  std::lock_guard<std::mutex> lock(mutex);
  return !error_message.is_empty();
}

String YAMLResult::get_error() const
{
  std::lock_guard<std::mutex> lock(mutex);
  if (error_line >= 0 && error_column >= 0) {
    return vformat("%s at line %d, column %d", error_message, error_line, error_column);
  }
  return error_message;
}

Variant YAMLResult::get_data() const
{
  std::lock_guard<std::mutex> lock(mutex);
  return data;
}

#ifndef YAML_RESULT_H
#define YAML_RESULT_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/variant.hpp>

#include <memory>
#include <mutex>
#include <optional>
#include <string>

namespace godot {

class YAMLParser;

class YAMLResult : public RefCounted {
  GDCLASS(YAMLResult, RefCounted)

  friend class YAMLParser; // Allow YAMLParser to access private members
  friend class YAMLEmitter; // Allow YAMLEmitter to access private members

  protected:
  static void _bind_methods();

  public:
  YAMLResult();

  void reset();
  void set_error(const String& msg, int line = -1, int column = -1);
  bool has_error() const;
  String get_error() const;
  Variant get_data() const;

  private:
  mutable std::mutex mutex;
  String error_message;
  int error_line;
  int error_column;
  Variant data;
};

} // namespace godot

#endif // YAML_RESULT_H

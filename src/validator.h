#ifndef YAML_VALIDATOR_H
#define YAML_VALIDATOR_H

#include "exception.h"
#include "result.h"
#include "yaml.h"
#include <memory>
#include <ryml.hpp>

namespace godot {

class YAML::Validator {
  public:
  Validator();
  ~Validator() = default;

  // Non-copyable
  Validator(const Validator&) = delete;
  Validator& operator=(const Validator&) = delete;

  // Quick validation without full parsing
  Ref<YAMLResult> validate(const String& input);

  private:
  ryml::Tree tree;
  ryml::Callbacks callbacks;
  std::unique_ptr<ryml::EventHandlerTree> evt_handler;
  std::unique_ptr<ryml::Parser> ryml_parser;
  Ref<YAMLResult> current_result;

  static void error_callback(const char* msg, size_t len, ryml::Location loc, void* user_data);
};

} // namespace godot

#endif // YAML_VALIDATOR_H

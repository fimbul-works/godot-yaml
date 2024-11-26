#ifndef YAML_REGISTER_TYPES_H
#define YAML_REGISTER_TYPES_H

#include <godot_cpp/core/class_db.hpp>

namespace godot {

void initialize_yaml_module(ModuleInitializationLevel p_level);
void uninitialize_yaml_module(ModuleInitializationLevel p_level);

} // namespace godot

#endif // YAML_REGISTER_TYPES_H

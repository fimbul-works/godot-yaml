#ifndef YAML_REGISTER_TYPES_H
#define YAML_REGISTER_TYPES_H

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void initialize_yaml_module(ModuleInitializationLevel p_level);
void uninitialize_yaml_module(ModuleInitializationLevel p_level);

#endif // YAML_REGISTER_TYPES_H

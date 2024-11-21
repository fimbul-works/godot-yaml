#include "register_types.h"

#include "yaml.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_yaml_module(ModuleInitializationLevel p_level)
{
  if (p_level != MODULE_INITIALIZATION_LEVEL_EDITOR) {
    return;
  }

  GDREGISTER_CLASS(YAMLResult);
  GDREGISTER_CLASS(YAMLEmitter);
  GDREGISTER_CLASS(YAMLParser);
  GDREGISTER_CLASS(YAML);
}

void uninitialize_yaml_module(ModuleInitializationLevel p_level)
{
  if (p_level != MODULE_INITIALIZATION_LEVEL_EDITOR) {
    return;
  }
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT yaml_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization* r_initialization)
{
  godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

  init_obj.register_initializer(initialize_yaml_module);
  init_obj.register_terminator(uninitialize_yaml_module);
  init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_EDITOR);

  return init_obj.init();
}
}

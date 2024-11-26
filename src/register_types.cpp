#include "register_types.h"

#include "variant_converter_registry.h"
#include "yaml.h"
#include "yaml_emitter.h"
#include "yaml_parser.h"
#include "yaml_result.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

namespace godot {

void initialize_yaml_module(ModuleInitializationLevel p_level)
{
  if (p_level != MODULE_INITIALIZATION_LEVEL_EDITOR) {
    return;
  }

  VariantConverterRegistry::initialize_registry();

  //ClassDB::register_class<YAML>();
  GDREGISTER_CLASS(YAMLResult);
  GDREGISTER_CLASS(YAMLFormat);
  GDREGISTER_CLASS(YAMLParser);
  GDREGISTER_CLASS(YAMLEmitter);
  GDREGISTER_CLASS(YAML);
}

void uninitialize_yaml_module(ModuleInitializationLevel p_level)
{
  if (p_level != MODULE_INITIALIZATION_LEVEL_EDITOR) {
    return;
  }

  VariantConverterRegistry::cleanup_registry();
}

} // namespace godot

extern "C" {
GDExtensionBool GDE_EXPORT yaml_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address,
        const GDExtensionClassLibraryPtr p_library,
        GDExtensionInitialization* r_initialization)
{
  godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

  init_obj.register_initializer(godot::initialize_yaml_module);
  init_obj.register_terminator(godot::uninitialize_yaml_module);
  init_obj.set_minimum_library_initialization_level(godot::MODULE_INITIALIZATION_LEVEL_EDITOR);

  return init_obj.init();
}
}

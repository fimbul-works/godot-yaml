#include "register_types.h"

#include "result.h"
#include "security.h"
#include "style.h"
#include "yaml.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

namespace godot {

void initialize_yaml_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	if (!ClassDB::class_exists("YAMLResult")) {
		GDREGISTER_CLASS(YAMLResult);
	}

	if (!ClassDB::class_exists("YAMLStyle")) {
		GDREGISTER_CLASS(YAMLStyle);
	}

	if (!ClassDB::class_exists("YAMLSecurity")) {
		GDREGISTER_CLASS(YAMLSecurity);
	}

	if (!ClassDB::class_exists("YAML")) {
		GDREGISTER_CLASS(YAML);
	}
}

void uninitialize_yaml_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	YAMLSecurity::cleanup_default_instance();
}

} // namespace godot

extern "C" {
GDExtensionBool GDE_EXPORT yaml_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address,
		const GDExtensionClassLibraryPtr p_library,
		GDExtensionInitialization *r_initialization) {
	godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

	init_obj.register_initializer(godot::initialize_yaml_module);
	init_obj.register_terminator(godot::uninitialize_yaml_module);
	init_obj.set_minimum_library_initialization_level(godot::MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
}

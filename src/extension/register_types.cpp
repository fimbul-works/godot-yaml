#include "register_types.hpp"

#include "../parser/security.hpp"
#include "../result.hpp"
#include "../style/style.hpp"
#include "../yaml.hpp"
#include <schema.hpp>
#include <validation_result.hpp>

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

	if (!ClassDB::class_exists("Schema")) {
		GDREGISTER_CLASS(Schema);
	}

	if (!ClassDB::class_exists("SchemaValidationResult")) {
		GDREGISTER_CLASS(SchemaValidationResult);
	}

	// Preload and register the JSON Schema Draft-07 and 2020-12 meta-schemas
	Schema::load_from_json_file("res://addons/yaml/schema/json_schema_draft_07.json", false);
	Schema::load_from_json_file("res://addons/yaml/schema/json_schema_draft_2020-12.json", false);
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
		const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

	init_obj.register_initializer(godot::initialize_yaml_module);
	init_obj.register_terminator(godot::uninitialize_yaml_module);
	init_obj.set_minimum_library_initialization_level(godot::MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
}

#include "yaml.h"
#include "version.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void YAML::_bind_methods() {
  	ClassDB::bind_method(D_METHOD("version"), &YAML::version);
}

YAML::YAML() {
	// Initialize any variables here.
	time_passed = 0.0;
}

YAML::~YAML() {
	// Add your cleanup here.
}

String YAML::version() {
  return String("Version " + String(GODOT_YAML_VERSION) + " (test)");
}

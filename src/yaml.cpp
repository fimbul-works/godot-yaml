#include "yaml.h"

// #include "ryml_extras.h"

#include <godot_cpp/core/class_db.hpp>

#include "version.h"

using namespace godot;

void YAML::_bind_methods()
{
  ClassDB::bind_method(D_METHOD("version"), &YAML::version);
  ClassDB::bind_method(D_METHOD("parse", "input"), &YAML::parse);
  ClassDB::bind_method(D_METHOD("stringify", "input"), &YAML::stringify);
  ClassDB::bind_method(D_METHOD("get_error"), &YAML::get_error);
}

YAML::YAML() :
        m_callbacks(nullptr, nullptr, nullptr, &YAML::error_callback),
        m_tree(m_callbacks),
        m_evt_handler(std::make_unique<ryml::EventHandlerTree>(m_callbacks)),
        m_parser(std::make_unique<ryml::Parser>(m_evt_handler.get(), ryml::ParserOptions().locations(true)))
{
  try {
    ryml::set_callbacks(m_callbacks);
  } catch (const std::exception& e) {
    godot::UtilityFunctions::printerr("Godot YAML initialization failed: ", e.what());
  } catch (...) {
    godot::UtilityFunctions::printerr("Godot YAML initialization failed: unknown error");
  }
}

YAML::~YAML()
{
  ryml::reset_callbacks();
}

String YAML::version()
{
#ifdef GODOT_YAML_DEBUG
  String target = "debug";
#else
  String target = "release";
#endif
  return String("Version " + String(GODOT_YAML_VERSION) + " (" + target + ")");
}

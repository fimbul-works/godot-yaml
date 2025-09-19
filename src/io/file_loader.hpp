/**
 * @file file_loader.hpp
 * @brief Defines the YAMLLoader class for loading YAML files.
 *
 * This file contains the YAMLLoader class which provides static methods for
 * loading and parsing YAML files from disk. It follows the same pattern as
 * Godot's ResourceLoader class with static method interfaces.
 */
#pragma once

#include "../parser/parser.hpp"
#include "../parser/security.hpp"
#include "../result.hpp"
#include "../util/string_hash.hpp"
#include "../yaml.hpp"

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <unordered_set>

namespace godot {

/**
 * @class YAML::FileLoader
 * @brief Provides static methods for loading YAML files.
 *
 * The YAMLLoader class follows the same pattern as Godot's ResourceLoader,
 * providing static methods for loading YAML files with proper error handling,
 * security controls, and cyclical reference detection.
 */
class YAML::FileLoader {
public:
	/**
	 * @brief Loads and parses a YAML file.
	 *
	 * @param path Path to the YAML file
	 * @param security Optional security settings for resource loading
	 * @param detect_style Whether to detect and store style information
	 * @return Ref<YAMLResult> Result containing parsed data or error information
	 */
	static Ref<YAMLResult> load_file(const String &path, const Ref<YAMLSecurity> security = nullptr, const bool detect_style = false);

	/**
	 * @brief Internal method for loading files within existing parser contexts.
	 *
	 * Used internally by the parser for handling cross-references and cyclical detection.
	 *
	 * @param path Path to the YAML file
	 * @param security_view Security settings for resource loading
	 * @param loading_yaml_paths Set for tracking cyclical references
	 * @return Ref<YAMLResult> Result containing parsed data or error information
	 */
	static Ref<YAMLResult> parser_load_file(const String &path, const YAMLSecurity::View &security_view, std::unordered_set<String, StringHasher, StringEqual> *loading_yaml_paths);

private:
	/**
	 * @brief Reads file content from disk with proper error handling.
	 *
	 * @param path Path to the file
	 * @param[out] content The file content if successful
	 * @return Ref<YAMLResult> Success result or error information
	 */
	static Ref<YAMLResult> read_file_content(const String &path, String &content);
};

} //namespace godot

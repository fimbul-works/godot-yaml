/**
 * @file file_saver.hpp
 * @brief Defines the YAML::FileSaver class for saving YAML files.
 *
 * This file contains the YAMLSaver class which provides static methods for
 * serializing Godot Variants to YAML format and saving them to disk. It
 * follows the same pattern as Godot's ResourceSaver class.
 */
#pragma once

#include "../emitter/emitter.hpp"
#include "../result.hpp"
#include "../style/style.hpp"
#include "../style/style_view.hpp"
#include "../yaml.hpp"

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

/**
 * @class YAML::FileSaver
 * @brief Provides static methods for saving Godot Variants to YAML files.
 *
 * The YAMLSaver class follows the same pattern as Godot's ResourceSaver,
 * providing static methods for serializing Godot data structures to YAML
 * format and writing them to disk. It supports style customization and
 * provides detailed error reporting.
 */
class YAML::FileSaver {
public:
	/**
	 * @brief Serializes data and saves it to a YAML file.
	 *
	 * @param data The Variant data to serialize
	 * @param path Path where the YAML file will be saved
	 * @param style Optional style settings for formatting the output
	 * @return Ref<YAMLResult> Result containing the YAML content or error information
	 */
	static Ref<YAMLResult> save_file(const Variant &data, const String &path, const Ref<YAMLStyle> &style = nullptr);

	/**
	 * @brief Serializes data to YAML format without writing to file.
	 *
	 * @param data The Variant data to serialize
	 * @param style Optional style settings for formatting the output
	 * @return Ref<YAMLResult> Result containing the YAML string or error information
	 */
	static Ref<YAMLResult> stringify(const Variant &data, const Ref<YAMLStyle> &style = nullptr);

private:
	/**
	 * @brief Writes YAML content to a file with proper error handling.
	 *
	 * @param yaml_content The YAML string to write
	 * @param path Path where the file will be saved
	 * @return Ref<YAMLResult> Success result with YAML content or error information
	 */
	static Ref<YAMLResult> write_file_content(const String &yaml_content, const String &path);
};

} //namespace godot

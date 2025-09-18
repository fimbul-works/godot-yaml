/**
 * @file parser.hpp
 * @brief YAML parser implementation for Godot.
 *
 * This file defines the YAMLParser class which processes YAML input strings
 * and converts them to Godot Variant types. It uses the ryml (RapidYAML) library
 * for low-level parsing and implements the conversion logic between YAML and
 * Godot types.
 */
#pragma once

#include "../style/style.hpp"
#include "../util/string_hash.hpp"
#include "parser_context.hpp"
#include "result.hpp"
#include "security.hpp"
#include "variant_converters/variant_converter_factory.hpp"

#include <ryml.hpp>

#include <memory>
#include <optional>
#include <stack>

namespace godot {

/**
 * @class YAMLParser
 * @brief YAMLParser for YAML documents that converts YAML to Godot Variants.
 *
 * The YAMLParser class handles the conversion of YAML documents to Godot's native
 * data types (Variants). It supports parsing of scalar values, sequences, and
 * mappings, as well as tagged values and custom class deserialization.
 *
 * Features:
 * - YAML parsing using the ryml library
 * - Conversion of YAML nodes to Godot types
 * - Support for custom tags and class deserialization
 * - Style detection and preservation
 * - Security features for resource loading
 * - Error handling with detailed diagnostics
 */
class YAMLParser {
public:
	/**
	 * @brief Constructs a new YAMLParser instance.
	 *
	 * @param shared_paths Optional set of paths for shared YAML loading
	 */
	YAMLParser(std::unordered_set<String, StringHasher, StringEqual> *shared_paths = nullptr);

	/**
	 * @brief Destructor to clean up.
	 */
	~YAMLParser();

	/**
	 * @brief Non-copyable class.
	 */
	YAMLParser(const YAMLParser &) = delete;
	YAMLParser &operator=(const YAMLParser &) = delete;

	/**
	 * @brief Parses a YAML string into a Godot Variant.
	 *
	 * @param input The YAML string to parse
	 * @param security_view Security settings for resource loading
	 * @param detect_style Whether to detect and preserve YAML style information
	 * @return Ref<YAMLResult> The parsed data or error information
	 */
	Ref<YAMLResult> parse(const String &input, const YAMLSecurity::View &security_view = YAMLSecurity::get_default_view(), const bool detect_style = false);

private:
	/**
	 * @brief YAMLParser components from ryml library.
	 */
	ryml::Tree tree;
	ryml::Callbacks callbacks;
	std::unique_ptr<ryml::EventHandlerTree> evt_handler;
	std::unique_ptr<ryml::Parser> ryml_parser;

	/**
	 * @brief Converter instances for different types.
	 */
	YAMLVariantConverterFactory factory;
	std::unordered_map<Variant::Type, std::unique_ptr<YAMLVariantConverter>> type_converters;
	std::unordered_map<String, YAMLVariantConverter *, StringHasher, StringEqual> tag_converters;

	/**
	 * @brief Current parsing state.
	 */
	Ref<YAMLResult> current_result;
	std::unique_ptr<YAMLParserContext> context;
	Ref<YAMLStyle> style;
	bool detect_style = false;

	/**
	 * @brief Tracks currently loading YAML paths to detect cyclical references.
	 */
	std::unordered_set<String, StringHasher, StringEqual> *loading_yaml_paths;
	bool owns_yaml_paths; // Whether this parser owns the set

	/**
	 * @brief Callback for error handling during parsing.
	 *
	 * @param msg Error message
	 * @param len Length of error message
	 * @param loc Location in the source document
	 * @param user_data User data pointer (typically the ryml::Parser instance)
	 */
	static void error_callback(const char *msg, size_t len, ryml::Location loc, void *user_data);

	/**
	 * @brief Node processing methods.
	 */

	/**
	 * @brief Processes a YAML node and converts it to a Godot Variant.
	 *
	 * @param node The YAML node to process
	 * @return Variant The resulting Godot value
	 */
	Variant process_node(const ryml::ConstNodeRef &node) const;

	/**
	 * @brief Common node processing logic for different node types.
	 *
	 * @param node The YAML node to process
	 * @return Variant The resulting Godot value
	 */
	Variant process_common(const ryml::ConstNodeRef &node) const;

	/**
	 * @brief Processes a map node and converts it to a Godot Dictionary.
	 *
	 * @param node The YAML map node
	 * @return Variant A Dictionary containing the map's key-value pairs
	 */
	Variant process_map(const ryml::ConstNodeRef &node) const;

	/**
	 * @brief Processes a sequence node and converts it to a Godot Array.
	 *
	 * @param node The YAML sequence node
	 * @return Variant An Array containing the sequence items
	 */
	Variant process_sequence(const ryml::ConstNodeRef &node) const;

	/**
	 * @brief Processes a scalar value node and converts it to an appropriate Godot type.
	 *
	 * @param node The YAML scalar node
	 * @return Variant The converted value (String, int, float, bool, etc.)
	 */
	Variant process_value(const ryml::ConstNodeRef &node) const;

	/**
	 * @brief Value parsing helper methods.
	 */

	/**
	 * @brief Attempts to parse a node with a YAML tag.
	 *
	 * @param node The tagged YAML node
	 * @return std::optional<Variant> The parsed value if successful, nullopt otherwise
	 */
	std::optional<Variant> try_parse_tagged_value(const ryml::ConstNodeRef &node) const;

	/**
	 * @brief Attempts to parse special YAML values like null, true, false, .inf, .nan.
	 *
	 * @param str_val The string value to parse
	 * @return std::optional<Variant> The special value if recognized, nullopt otherwise
	 */
	std::optional<Variant> try_parse_special_value(const String &str_val) const;

	/**
	 * @brief Attempts to parse a numeric value from a string.
	 *
	 * @param str_val The string representation
	 * @param val The original ryml string view
	 * @return std::optional<Variant> The numeric value if valid, nullopt otherwise
	 */
	std::optional<Variant> try_parse_numeric_value(const String &str_val, const ryml::csubstr &val) const;

	/**
	 * @brief Extracts the tag from a YAML node.
	 *
	 * @param node The YAML node
	 * @return String The tag without the '!' prefix
	 */
	String extract_tag(const ryml::ConstNodeRef &node) const;

	/**
	 * @brief Extracts the key from a YAML node.
	 *
	 * @param node The YAML node with a key
	 * @return String The key as a Godot String
	 */
	String extract_key(const ryml::ConstNodeRef &node) const;

	/**
	 * @brief Object and resource handling methods.
	 */

	/**
	 * @brief Parses an object or resource declaration.
	 *
	 * @param node The YAML node containing the object data
	 * @param class_name The class name to instantiate
	 * @return Variant The created object or resource
	 */
	Variant parse_object_or_resource(const ryml::ConstNodeRef &node, const String &class_name) const;

	/**
	 * @brief Loads a resource from a path.
	 *
	 * @param path The resource path
	 * @param node The original YAML node (for error reporting)
	 * @return Variant The loaded resource
	 */
	Variant load_resource(const String &path, const ryml::ConstNodeRef &node) const;

	/**
	 * @brief Populates an object's properties from a YAML map.
	 *
	 * @param obj The object to populate
	 * @param node The YAML map containing property values
	 */
	void populate_object_properties(Object *obj, const ryml::ConstNodeRef &node) const;

	/**
	 * @brief Initializes the type converters.
	 */
	void init_converters();

	/**
	 * @brief Converter access helper methods.
	 */

	/**
	 * @brief Gets a converter for a specific Variant type.
	 *
	 * @param type The Variant type
	 * @return YAMLVariantConverter* The converter or nullptr if not found
	 */
	YAMLVariantConverter *get_converter_for_type(Variant::Type type) const;

	/**
	 * @brief Gets a converter for a specific YAML tag.
	 *
	 * @param tag The tag name
	 * @return YAMLVariantConverter* The converter or nullptr if not found
	 */
	YAMLVariantConverter *get_converter_for_tag(const String &tag) const;

	/**
	 * @brief Security view for resource access control.
	 */
	YAMLSecurity::View security_view;
};

} // namespace godot

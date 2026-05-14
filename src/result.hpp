/**
 * @file result.hpp
 * @brief Defines the YAMLResult class for YAML operation results.
 *
 * This file contains the YAMLResult class which encapsulates the result of
 * YAML operations such as parsing, serialization, and validation. It can
 * store either successful data or error information, providing a uniform
 * interface for handling operation outcomes.
 */
#pragma once

#include "style/style.hpp"
#include <validation_result.hpp>

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/variant.hpp>

namespace godot {

/**
 * @class YAMLResult
 * @brief Encapsulates the result of YAML operations.
 *
 * The YAMLResult class provides a consistent way to handle both successful
 * and failed YAML operations. It can store:
 * - Parsed data as a Godot Variant (single document)
 * - Multiple YAML documents as an Array
 * - Error information (message, line, column)
 * - Style information for formatting
 *
 * The class explicitly distinguishes between single documents and multi-document
 * results to avoid ambiguity when the single document happens to be an Array.
 *
 * @extends RefCounted
 */
class YAMLResult : public RefCounted {
	GDCLASS(YAMLResult, RefCounted)

protected:
	/**
	 * @brief Binds methods to make them accessible from GDScript.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Default constructor that creates an empty successful result.
	 */
	YAMLResult() :
			data(Variant()), is_multi_document(false), error_message(""), error_line(-1), error_column(-1) {}

	/**
	 * @brief Creates a successful result with single document data.
	 *
	 * @param data The parsed or processed data (single document)
	 * @param style Optional style information
	 * @return Ref<YAMLResult> A successful single-document result object
	 */
	static Ref<YAMLResult> success(const Variant &data, const Ref<YAMLStyle> &style = nullptr, const Ref<SchemaValidationResult> &validation = nullptr);

	/**
	 * @brief Creates a successful result with multiple documents.
	 *
	 * @param documents Array containing multiple document data
	 * @return Ref<YAMLResult> A successful multi-document result object
	 */
	static Ref<YAMLResult> multi_document_success(const Array &documents);

	/**
	 * @brief Creates an error result with location information.
	 *
	 * @param msg The error message
	 * @param line Line number where the error occurred (-1 if unknown)
	 * @param column Column number where the error occurred (-1 if unknown)
	 * @return Ref<YAMLResult> An error result object
	 */
	static Ref<YAMLResult> error(const String &msg, int64_t line = -1, int64_t column = -1);

	/**
	 * @brief Creates a user-defined error result.
	 *
	 * This is similar to error() but is intended for user-defined errors
	 * rather than parsing errors.
	 *
	 * @param msg The error message
	 * @return Ref<YAMLResult> An error result object
	 */
	static Ref<YAMLResult> user_error(const String &msg);

	/**
	 * @brief Gets the result data.
	 *
	 * Returns the data contained in this result:
	 * - For parsing operations: the parsed Variant data
	 * - For emission operations: the generated YAML string
	 *
	 * For single documents, returns the data directly.
	 * For multi-document results, returns the first document and logs a warning.
	 * Use get_document() or get_documents() for explicit multi-document access.
	 *
	 * @return Variant The result data or first document data
	 */
	Variant get_data() const;

	/**
	 * @brief Get a specific document by index.
	 * @param index Document index (0 for first document)
	 * @return Variant The document data or null if the index is invalid
	 */
	Variant get_document(int index = 0) const;

	/**
	 * @brief Gets the number of documents in the result.
	 * @return int Number of documents (0 if an error occurred, 1 for single documents)
	 */
	int get_document_count() const;

	/**
	 * @brief Gets all documents as an Array.
	 *
	 * For single documents, returns an Array containing the single document.
	 * For multi-document results, returns the documents Array directly.
	 *
	 * @return Array An array containing all document data
	 */
	Array get_documents() const;

	/**
	 * @brief Checks if this result contains multiple documents.
	 * @return bool True if this result contains multiple documents
	 */
	bool has_multiple_documents() const;

	/**
	 * @brief Checks if the result contains an error.
	 * @return bool True if an error occurred, false otherwise
	 */
	bool has_error() const;

	/**
	 * @brief Gets the error message.
	 * @return String The error message or empty if no error
	 */
	String get_error_message() const;

	/**
	 * @brief Gets the line number where the error occurred.
	 * @return int64_t Line number or -1 if not applicable
	 */
	int64_t get_error_line() const;

	/**
	 * @brief Gets the column number where the error occurred.
	 * @return int64_t Column number or -1 if not applicable
	 */
	int64_t get_error_column() const;

	/**
	 * @brief Gets a formatted error string with location information if applicable.
	 * @return String Formatted error message with line and column if applicable
	 */
	String get_error() const;

	/**
	 * @brief Style handling methods.
	 */

	/**
	 * @brief Checks if the result contains style information.
	 * @return bool True if style information is available
	 */
	bool has_style() const;

	/**
	 * @brief Gets the style information.
	 * @return Ref<YAMLStyle> The style object or null if not available
	 */
	Ref<YAMLStyle> get_style() const;

	/**
	 * @brief Checks if the result contains schema validation information.
	 */
	bool has_validation_result() const;

	/**
	 * @brief Gets the schema validation result.
	 * @return Ref<SchemaValidationResult> The validation result or null if not available
	 */
	Ref<SchemaValidationResult> get_validation_result() const;

	/**
	 * @brief Checks if there are validation errors.
	 */
	bool has_validation_errors() const;

	/**
	 * @brief Gets the number of validation errors.
	 */
	int get_validation_error_count() const;

	/**
	 * @brief Gets a summary of validation results.
	 * @return String description of validation results
	 */
	String get_validation_summary() const;

	/**
	 * @brief Gets all validation errors as an Array of Dictionaries.
	 * Each Dictionary contains keys like "message", "path", "constraint", and "value".
	 * @return Array of validation error Dictionaries
	 */
	Array get_validation_errors() const;

	/**
	 * @brief Converts the result to a string representation.
	 *
	 * @return String String representation of the result
	 */
	String _to_string() const;

private:
	/**
	 * @brief Private constructor to enforce factory method usage.
	 *
	 * @param data_ The result data
	 * @param is_multi_document_ Whether this contains multiple documents
	 * @param style_ Optional style information
	 * @param error_ Optional error message
	 * @param line Optional error line number
	 * @param col Optional error column number
	 */
	YAMLResult(const Variant &data_, bool is_multi_document_, const Ref<YAMLStyle> &style_ = nullptr, const String &error_ = "", int64_t line = -1, int64_t col = -1, const Ref<SchemaValidationResult> &validation_ = nullptr) :
			data(data_),
			is_multi_document(is_multi_document_),
			style(style_),
			error_message(error_),
			error_line(line),
			error_column(col),
			validation_result(validation_) {}

	/**
	 * @brief Immutable state members.
	 */
	const Variant data; ///< The result data (single document) or documents Array (multi-document)
	const bool is_multi_document; ///< Whether this result contains multiple documents
	const String error_message; ///< Error message if an error occurred
	const int64_t error_line; ///< Line number where the error occurred
	const int64_t error_column; ///< Column number where the error occurred
	const Ref<YAMLStyle> style; ///< Style information if available
	const Ref<SchemaValidationResult> validation_result; ///< Schema validation result if available
};

} // namespace godot

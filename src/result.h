/**
 * @file result.h
 * @brief Defines the YAMLResult class for YAML operation results.
 *
 * This file contains the YAMLResult class which encapsulates the result of
 * YAML operations such as parsing, serialization, and validation. It can
 * store either successful data or error information, providing a uniform
 * interface for handling operation outcomes.
 */

#ifndef YAML_RESULT_H
#define YAML_RESULT_H

#include "style.h"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/variant.hpp>

namespace godot {

// Forward reference
class YAMLStyle;

/**
 * @class YAMLResult
 * @brief Encapsulates the result of YAML operations.
 *
 * The YAMLResult class provides a consistent way to handle both successful
 * and failed YAML operations. It can store:
 * - Parsed data as a Godot Variant
 * - Error information (message, line, column)
 * - Style information for formatting
 *
 * It supports multiple YAML documents within a single result and
 * provides access to the result data with appropriate error handling.
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
			data(Variant()), error_message(""), error_line(-1), error_column(-1) {}

	/**
	 * @brief Static factory methods to create result objects.
	 */

	/**
	 * @brief Creates a successful result with data.
	 *
	 * @param data The parsed or processed data
	 * @param style Optional style information
	 * @return Ref<YAMLResult> A successful result object
	 */
	static Ref<YAMLResult> success(const Variant &data, const Ref<YAMLStyle> &style = nullptr);

	/**
	 * @brief Creates an error result with location information.
	 *
	 * @param msg The error message
	 * @param line Line number where the error occurred (-1 if unknown)
	 * @param column Column number where the error occurred (-1 if unknown)
	 * @return Ref<YAMLResult> An error result object
	 */
	static Ref<YAMLResult> error(const String &msg, int line = -1, int column = -1);

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
	 * @brief Immutable accessors for result data.
	 */

	/**
	 * @brief Gets the data for a specific document index.
	 *
	 * @param index Document index (0 for single documents)
	 * @return Variant The data or null if the index is invalid
	 */
	Variant get_data(int index = 0) const;

	/**
	 * @brief Alias for get_data() for clarity when working with multiple documents.
	 *
	 * @param index Document index (0 for single documents)
	 * @return Variant The document data or null if the index is invalid
	 */
	Variant get_document(int index = 0) const { return get_data(index); }

	/**
	 * @brief Gets the number of documents in the result.
	 *
	 * @return int Number of documents (0 if an error occurred)
	 */
	int get_document_count() const;

	/**
	 * @brief Checks if the result contains an error.
	 *
	 * @return bool True if an error occurred, false otherwise
	 */
	bool has_error() const { return !error_message.is_empty(); }

	/**
	 * @brief Gets the error message.
	 *
	 * @return String The error message or empty if no error
	 */
	String get_error_message() const { return error_message; }

	/**
	 * @brief Gets the line number where the error occurred.
	 *
	 * @return int Line number or -1 if not applicable
	 */
	int get_error_line() const { return error_line; }

	/**
	 * @brief Gets the column number where the error occurred.
	 *
	 * @return int Column number or -1 if not applicable
	 */
	int get_error_column() const { return error_column; }

	/**
	 * @brief Gets a formatted error string with location information.
	 *
	 * @return String Formatted error message with line and column
	 */
	String get_error() const { return vformat("%s (line %d, column %d)", error_message, error_line, error_column); }

	/**
	 * @brief Style handling methods.
	 */

	/**
	 * @brief Checks if the result contains style information.
	 *
	 * @return bool True if style information is available
	 */
	bool has_style() const { return style.is_valid(); }

	/**
	 * @brief Gets the style information.
	 *
	 * @return Ref<YAMLStyle> The style object or null if not available
	 */
	Ref<YAMLStyle> get_style() const { return style; }

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
	 * @param style_ Optional style information
	 * @param error_ Optional error message
	 * @param line Optional error line number
	 * @param col Optional error column number
	 */
	YAMLResult(
			const Variant &data_,
			const Ref<YAMLStyle> &style_ = nullptr,
			const String &error_ = "",
			int line = -1,
			int col = -1) :
			data(data_), style(style_), error_message(error_), error_line(line), error_column(col) {}

	/**
	 * @brief Immutable state members.
	 */
	const Variant data; ///< The result data (parsed YAML or serialized string)
	const String error_message; ///< Error message if an error occurred
	const int error_line; ///< Line number where the error occurred
	const int error_column; ///< Column number where the error occurred
	const Ref<YAMLStyle> style; ///< Style information if available
};

} // namespace godot

#endif // YAML_RESULT_H

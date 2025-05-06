/**
 * @file style_view.h
 * @brief Defines an immutable view of YAMLStyle for thread-safe operations.
 *
 * This file contains the YAMLStyle::View class which provides a read-only
 * view of YAMLStyle objects. This is particularly useful for thread-safe
 * operations during YAML emission where style information should not be modified.
 */

#ifndef YAML_STYLE_VIEW_H
#define YAML_STYLE_VIEW_H

#include "string_hash.h"
#include "style.h"

#include <godot_cpp/variant/dictionary.hpp>
#include <ryml.hpp>

#include <memory>
#include <unordered_map>

namespace godot {

/**
 * @class YAMLStyle::View
 * @brief Provides an immutable view of YAMLStyle objects.
 *
 * The View class offers a thread-safe, read-only interface to YAMLStyle
 * settings. It captures a snapshot of style settings at creation time
 * and provides methods for applying those settings during YAML emission.
 *
 * This separation of mutable YAMLStyle and immutable View helps ensure
 * thread safety and prevents unintended modifications during YAML operations.
 */
class YAMLStyle::View {
public:
	/**
	 * @brief Default constructor creates an empty view.
	 */
	View() = default;

	/**
	 * @brief Checks if the view is valid (contains data).
	 *
	 * @return bool True if the view contains valid style data
	 */
	bool is_valid() const;

	/**
	 * @brief Creates a view from a YAMLStyle object.
	 *
	 * @param style The source style object (can be null)
	 * @return View A new view containing a snapshot of the style settings
	 */
	static View create_view(const Ref<YAMLStyle> &style = Ref<YAMLStyle>());

	/**
	 * @brief Accessors for style settings.
	 */

	/**
	 * @brief Gets the container form.
	 *
	 * @return YAMLStyle::ContainerForm The container form setting
	 */
	YAMLStyle::ContainerForm get_container_form() const;

	/**
	 * @brief Gets the flow style.
	 *
	 * @return YAMLStyle::FlowStyle The flow style setting
	 */
	YAMLStyle::FlowStyle get_flow_style() const;

	/**
	 * @brief Gets the string style.
	 *
	 * @return YAMLStyle::StringStyle The string style setting
	 */
	YAMLStyle::StringStyle get_string_style() const;

	/**
	 * @brief Gets the integer format.
	 *
	 * @return YAMLStyle::IntegerFormat The integer format setting
	 */
	YAMLStyle::IntegerFormat get_integer_format() const;

	/**
	 * @brief Gets the float format.
	 *
	 * @return YAMLStyle::FloatFormat The float format setting
	 */
	YAMLStyle::FloatFormat get_float_format() const;

	/**
	 * @brief Gets the binary encoding.
	 *
	 * @return YAMLStyle::BinaryEncoding The binary encoding setting
	 */
	YAMLStyle::BinaryEncoding get_binary_encoding() const;

	/**
	 * @brief Gets the template style for array items.
	 *
	 * @return YAMLStyle::View The template style or an empty view if not defined
	 */
	YAMLStyle::View get_template_style() const;

	/**
	 * @brief Helper methods for style properties.
	 */

	/**
	 * @brief Checks if using block string style.
	 *
	 * @return bool True if using literal or folded style
	 */
	bool is_block_style() const;

	/**
	 * @brief Checks if using quoted string style.
	 *
	 * @return bool True if using single or double quotes
	 */
	bool uses_quotes() const;

	/**
	 * @brief Checks if using flow (inline) style.
	 *
	 * @return bool True if using flow style
	 */
	bool uses_flow() const;

	/**
	 * @brief Style application helpers for ryml.
	 */

	/**
	 * @brief Applies flow style settings to a ryml node.
	 *
	 * @param node The node to apply flow style to
	 */
	void apply_flow_style(ryml::NodeRef &node) const;

	/**
	 * @brief Applies string style settings to a ryml node.
	 *
	 * @param node The node to apply string style to
	 */
	void apply_string_style(ryml::NodeRef &node) const;

	/**
	 * @brief Child style access.
	 */

	/**
	 * @brief Gets a child style view.
	 *
	 * @param key The child key
	 * @return View The child style view or an empty view if not found
	 */
	View get_child(const String &key) const;

	/**
	 * @brief Checks if a child style exists.
	 *
	 * @param key The child key
	 * @return bool True if the child exists
	 */
	bool has_child(const String &key) const;

	/**
	 * @brief Gets the custom settings dictionary.
	 *
	 * @return const Dictionary& Reference to the custom settings
	 */
	const Dictionary &get_custom_settings() const;

private:
	/**
	 * @struct ViewData
	 * @brief Internal structure for style data.
	 *
	 * Stores all style settings and child styles in an immutable format.
	 */
	struct ViewData {
		bool has_inherit_styles = false;
		bool inherit_styles = true;

		bool has_container_form = false;
		ContainerForm container_form;

		bool has_flow_style = false;
		FlowStyle flow_style;

		bool has_string_style = false;
		StringStyle string_style;

		bool has_integer_format = false;
		IntegerFormat integer_format;

		bool has_float_format = false;
		FloatFormat float_format;

		bool has_binary_encoding = false;
		BinaryEncoding binary_encoding;

		bool has_custom_settings = false;
		Dictionary custom_settings;
		std::unordered_map<String, std::shared_ptr<const ViewData>, StringHasher, StringEqual> children;
	};

	/**
	 * @brief Constructor that takes a shared ViewData instance.
	 *
	 * @param p_data The view data to use
	 */
	explicit View(std::shared_ptr<const ViewData> p_data);

	/**
	 * @brief Shared pointer to the immutable view data.
	 */
	std::shared_ptr<const ViewData> data;
};

} // namespace godot

#endif // YAML_STYLE_VIEW_H

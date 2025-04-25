#ifndef YAML_STYLE_VIEW_H
#define YAML_STYLE_VIEW_H

#include "string_hash.h"
#include "style.h"

#include <godot_cpp/variant/dictionary.hpp>
#include <ryml.hpp>

#include <memory>
#include <unordered_map>

namespace godot {

// Style view implementation that doesn't need complete YAMLStyle definition
class YAMLStyle::View {
public:
	// Default constructor creates an empty view
	View() = default;
	bool is_valid() const;

	static View create_view(const Ref<YAMLStyle> &style = Ref<YAMLStyle>());

	// Const accessors
	YAMLStyle::ContainerForm get_container_form() const;
	YAMLStyle::FlowStyle get_flow_style() const;
	YAMLStyle::StringStyle get_string_style() const;
	YAMLStyle::IntegerFormat get_integer_format() const;
	YAMLStyle::FloatFormat get_float_format() const;
	YAMLStyle::BinaryEncoding get_binary_encoding() const;
	YAMLStyle::View get_template_style() const;

	// Helper methods
	bool is_block_style() const;
	bool uses_quotes() const;
	bool uses_flow() const;

	// Style helpers
	void apply_flow_style(ryml::NodeRef &node) const;
	void apply_string_style(ryml::NodeRef &node) const;

	// Child style access
	View get_child(const String &key) const;
	bool has_child(const String &key) const;

	// Custom settings access
	const Dictionary &get_custom_settings() const;

private:
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

	explicit View(std::shared_ptr<const ViewData> p_data);

	std::shared_ptr<const ViewData> data;
};

} // namespace godot

#endif // YAML_STYLE_VIEW_H

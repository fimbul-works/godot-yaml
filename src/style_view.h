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

  static View create_view(const Ref<YAMLStyle>& style = Ref<YAMLStyle>());

  // Const accessors
  YAMLStyle::ScalarStyle get_scalar_style() const;
  YAMLStyle::QuoteStyle get_quote_style() const;
  YAMLStyle::ContainerForm get_container_form() const;
  YAMLStyle::FlowStyle get_flow_style() const;
  YAMLStyle::NumberFormat get_number_format() const;
  YAMLStyle::BinaryEncoding get_binary_encoding() const;
  YAMLStyle::ChompingStyle get_chomping_style() const;

  // Helper methods
  bool is_block_style() const;
  bool uses_quotes() const;
  bool uses_flow() const;

  // Style helpers
  void apply_scalar_style(ryml::NodeRef& node) const;
  void apply_quote_style(ryml::NodeRef& node) const;
  void apply_flow_style(ryml::NodeRef& node) const;

  // Child style access
  View get_child(const String& key) const;
  bool has_child(const String& key) const;

  // Custom settings access
  const Dictionary& get_custom_settings() const;

  // Debug helper
  String get_debug_string() const;

  private:
  struct ViewData {
    ScalarStyle scalar_style;
    QuoteStyle quote_style;
    ContainerForm container_form;
    FlowStyle flow_style;
    NumberFormat number_format;
    BinaryEncoding binary_encoding;
    ChompingStyle chomping_style;

    Dictionary custom_settings;
    std::unordered_map<String, std::shared_ptr<const ViewData>, StringHasher, StringEqual> children;
  };

  explicit View(std::shared_ptr<const ViewData> p_data);

  std::shared_ptr<const ViewData> data;
};

} // namespace godot

#endif // YAML_STYLE_VIEW_H

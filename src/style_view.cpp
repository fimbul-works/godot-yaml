#include "style_view.h"
#include "util_string.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <ryml.hpp>

using namespace godot;

// YAMLStyleView implementation
YAMLStyle::View::View(std::shared_ptr<const ViewData> p_data) :
        data(std::move(p_data))
{
}

YAMLStyle::View YAMLStyle::View::create_view(const Ref<YAMLStyle>& style)
{
  if (!style.is_valid()) {
    return YAMLStyle::View();
  }

  auto view_data = std::make_shared<ViewData>();
  if (!view_data) {
    return View();
  }

  // Copy all style values
  view_data->scalar_style = style->scalar_style;
  view_data->quote_style = style->quote_style;
  view_data->container_form = style->container_form;
  view_data->flow_style = style->flow_style;
  view_data->number_format = style->number_format;
  view_data->binary_encoding = style->binary_encoding;
  view_data->chomping_style = style->chomping_style;
  view_data->custom_settings = style->custom_settings;

  // Create views for all children
  Array keys = style->get_children_keys();
  for (int i = 0; i < keys.size(); i++) {
    String key = keys[i];
    if (key.is_empty()) {
      continue;
    }

    Ref<YAMLStyle> child = style->get_child(key);
    if (child.is_valid()) {
      auto child_view = create_view(child);
      if (child_view.is_valid()) {
        view_data->children[key] = child_view.data;
      }
    }
  }

  return YAMLStyle::View(std::move(view_data));
}

// YAMLStyleView accessors
YAMLStyle::ScalarStyle YAMLStyle::View::get_scalar_style() const
{
  return data ? data->scalar_style : YAMLStyle::SCALAR_ANY;
}

YAMLStyle::QuoteStyle YAMLStyle::View::get_quote_style() const
{
  return data ? data->quote_style : YAMLStyle::QUOTE_ANY;
}

YAMLStyle::ContainerForm YAMLStyle::View::get_container_form() const
{
  return data ? data->container_form : YAMLStyle::FORM_ANY;
}

YAMLStyle::FlowStyle YAMLStyle::View::get_flow_style() const
{
  return data ? data->flow_style : YAMLStyle::FLOW_ANY;
}

YAMLStyle::NumberFormat YAMLStyle::View::get_number_format() const
{
  return data ? data->number_format : YAMLStyle::NUM_ANY;
}

YAMLStyle::BinaryEncoding YAMLStyle::View::get_binary_encoding() const
{
  return data ? data->binary_encoding : YAMLStyle::BIN_ANY;
}

YAMLStyle::ChompingStyle YAMLStyle::View::get_chomping_style() const
{
  return data ? data->chomping_style : YAMLStyle::CHOMP_ANY;
}

YAMLStyle::View YAMLStyle::View::get_template_style() const
{
  if (data && data->children.count("_template")) {
    return View(data->children.at("_template"));
  }
  return View();
}

// Helper methods
bool YAMLStyle::View::is_block_style() const
{
  return get_scalar_style() == YAMLStyle::SCALAR_BLOCK || get_scalar_style() == YAMLStyle::SCALAR_LITERAL || get_scalar_style() == YAMLStyle::SCALAR_FOLDED;
}

bool YAMLStyle::View::uses_quotes() const
{
  return get_quote_style() == YAMLStyle::QUOTE_SINGLE || get_quote_style() == YAMLStyle::QUOTE_DOUBLE;
}

bool YAMLStyle::View::uses_flow() const
{
  return get_flow_style() == YAMLStyle::FLOW_SINGLE;
}

void YAMLStyle::View::apply_scalar_style(ryml::NodeRef& node) const
{
  // Only apply styles when requested
  if (!is_valid()) {
    return;
  }

  // Scalar styles
  if (data->scalar_style != YAMLStyle::SCALAR_ANY) {
    switch (data->scalar_style) {
      case YAMLStyle::SCALAR_BLOCK:
        node |= ryml::BLOCK;
        break;
      case YAMLStyle::SCALAR_LITERAL:
        node |= ryml::VAL_LITERAL | ryml::BLOCK;
        break;
      case YAMLStyle::SCALAR_FOLDED:
        node |= ryml::VAL_FOLDED;
        break;
      case YAMLStyle::SCALAR_PLAIN:
        break;
      default:
        break;
    }
  }
}

void YAMLStyle::View::apply_quote_style(ryml::NodeRef& node) const
{
  // Only apply styles when requested
  if (!is_valid()) {
    return;
  }

  // Handle quote style - this can be combined with scalar style
  if (data->quote_style != YAMLStyle::QUOTE_ANY) {
    // User explicitly specified quote style
    switch (data->quote_style) {
      case YAMLStyle::QUOTE_SINGLE:
        node |= ryml::VAL_SQUO;
        break;
      case YAMLStyle::QUOTE_DOUBLE:
        node |= ryml::VAL_DQUO;
        break;
      case YAMLStyle::QUOTE_NONE:
        // No quotes - no action needed
        break;
      default:
        break;
    }
  }
}

void YAMLStyle::View::apply_flow_style(ryml::NodeRef& node) const
{
  if (!is_valid()) {
    return;
  }

  if (data->flow_style == YAMLStyle::FLOW_SINGLE) {
    node |= ryml::FLOW_SL;
  }
}

bool YAMLStyle::View::is_valid() const
{
  return data != nullptr;
}

// Child management
YAMLStyle::View YAMLStyle::View::get_child(const String& key) const
{
  if (data && data->children.count(key)) {
    return YAMLStyle::View(data->children.at(key));
  }
  return YAMLStyle::View(nullptr);
}

bool YAMLStyle::View::has_child(const String& key) const
{
  return data && data->children.count(key);
}

const Dictionary& YAMLStyle::View::get_custom_settings() const
{
  static const Dictionary empty;
  return data ? data->custom_settings : empty;
}

String YAMLStyle::View::get_debug_string() const
{
  if (!data) {
    return "Invalid style view";
  }

  String debug;
  debug += "YAML Style View:\n";
  debug += "--------------\n";
  debug += vformat("Scalar Style:     %s\n", YAMLStyle::get_scalar_style_string(data->scalar_style));
  debug += vformat("Quote Style:      %s\n", YAMLStyle::get_quote_style_string(data->quote_style));
  debug += vformat("Collection Style: %s\n", YAMLStyle::get_container_form_string(data->container_form));
  debug += vformat("Flow Style:       %s\n", YAMLStyle::get_flow_style_string(data->flow_style));
  debug += vformat("Number Format:    %s\n", YAMLStyle::get_number_format_string(data->number_format));
  debug += vformat("Binary Encoding:  %s\n", YAMLStyle::get_binary_encoding_string(data->binary_encoding));
  debug += vformat("Chomping Style:   %s\n", YAMLStyle::get_chomping_style_string(data->chomping_style));

  if (!data->custom_settings.is_empty()) {
    debug += "\nCustom Settings:\n";
    Array keys = data->custom_settings.keys();
    for (int i = 0; i < keys.size(); i++) {
      debug += vformat("  %s: %s\n", String(keys[i]), String(data->custom_settings[keys[i]]));
    }
  }

  if (!data->children.empty()) {
    debug += "\nChild Views:\n";
    for (const auto& pair : data->children) {
      debug += vformat("  %s:\n", pair.first);
      YAMLStyle::View child_view(pair.second);
      String child_debug = child_view.get_debug_string();
      PackedStringArray lines = child_debug.split("\n");
      for (int i = 0; i < lines.size(); i++) {
        if (!lines[i].is_empty()) {
          debug += vformat("    %s\n", lines[i]);
        }
      }
    }
  }

  return debug;
}

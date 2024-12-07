#include "emitter.h"
#include "util_numeric.h"
#include "util_string.h"
#include "yaml.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

Ref<YAMLResult> YAMLEmitter::emit(const Variant& input, const Ref<YAMLStyle>& style)
{
  std::lock_guard<std::mutex> lock(emit_mutex);

  try {
    ryml::Tree tree;
    emit_value(tree.rootref(), input, style);

    std::string yaml_str = ryml::emitrs_yaml<std::string>(tree);
    return YAMLResult::success(String::utf8(yaml_str.c_str(), yaml_str.length()), style);

  } catch (const std::exception& e) {
    return YAMLResult::error(e.what());
  }
}

void YAMLEmitter::emit_value(ryml::NodeRef& node, const Variant& value, const Ref<YAMLStyle>& style)
{
  if (style.is_valid() && !style->custom_settings.is_empty()) {
    if (style->custom_settings.has("tag")) {
      String tag = style->custom_settings["tag"];
      node.set_val_tag(to_ryml_str("!" + tag));
    }
  }

  switch (value.get_type()) {
    case Variant::NIL:
      emit_nil(node);
      break;

    case Variant::BOOL:
      emit_bool(node, value);
      break;

    case Variant::INT:
    case Variant::FLOAT:
      emit_number(node, value, style);
      break;

    case Variant::STRING:
      emit_string(node, value, style);
      break;

    case Variant::ARRAY:
      emit_array(node, value, style);
      break;

    case Variant::DICTIONARY:
      emit_dictionary(node, value, style);
      break;

    default:
      if (VariantConverterRegistry::has_converter(value.get_type())) {
        if (const auto* converter = VariantConverterRegistry::get_converter(value.get_type())) {
          emit_tagged_value(node, value, style);
          return;
        }
      }

      String type_name = Variant::get_type_name(value.get_type());
      current_result = YAMLResult::error(vformat("Unsupported type: %s", type_name));
      break;
  }
}

void YAMLEmitter::emit_nil(ryml::NodeRef& node)
{
  ryml::csubstr null = {};
  node << null;
}

void YAMLEmitter::emit_bool(ryml::NodeRef& node, bool value)
{
  node << (value ? "true" : "false");
}

void YAMLEmitter::emit_number(ryml::NodeRef& node, const Variant& value, const Ref<YAMLStyle>& style)
{
  YAMLStyle::NumberFormat format = style.is_valid() ? style->number_format : YAMLStyle::NUM_DECIMAL;

  if (value.get_type() == Variant::INT) {
    int64_t int_val = static_cast<int64_t>(value.operator int64_t());
    node << int_to_string(int_val, format);
  } else {
    double float_val = static_cast<double>(value.operator double());
    node << float_to_string(float_val, format);
  }
}

void YAMLEmitter::emit_string(ryml::NodeRef& node, const String& value, const Ref<YAMLStyle>& style)
{
  if (value.is_empty()) {
    node << ryml::csubstr {};
    return;
  }

  // First handle explicit style settings if provided
  if (style.is_valid()) {
    switch (style->scalar_style) {
      case YAMLStyle::STYLE_BLOCK: {
        node |= ryml::BLOCK;

        // Set block style (literal or folded)
        if (style->block_style == YAMLStyle::BLOCK_LITERAL) {
          node |= ryml::VAL_LITERAL;
        } else if (style->block_style == YAMLStyle::BLOCK_FOLDED) {
          node |= ryml::VAL_FOLDED;
        }
        break;
      }
      case YAMLStyle::STYLE_QUOTED:
        node |= ryml::VAL_DQUO;
        break;
      case YAMLStyle::STYLE_PLAIN:
        // Only use block if needed
        if (needs_block_style(value)) {
          node |= ryml::BLOCK;
        }
        break;
      case YAMLStyle::STYLE_ANY:
        // Auto-detect appropriate style
        if (value.contains("\n")) {
          node |= ryml::BLOCK;
          // If the string has significant whitespace or ends with newlines,
          // use literal style, otherwise use folded
          if (value.ends_with("\n") || value.contains("  ")) {
            node |= ryml::VAL_LITERAL;
          } else {
            node |= ryml::VAL_FOLDED;
          }
        } else if (needs_block_style(value)) {
          node |= ryml::BLOCK;
        }
        break;
    }
  } else {
    // No style provided - use auto-detection
    if (value.contains("\n")) {
      node |= ryml::BLOCK;
      // If the string has significant whitespace or ends with newlines,
      // use literal style, otherwise use folded
      if (value.ends_with("\n") || value.contains("  ")) {
        node |= ryml::VAL_LITERAL;
      } else {
        node |= ryml::VAL_FOLDED;
      }
    } else if (needs_block_style(value)) {
      node |= ryml::BLOCK;
    }
  }

  node << to_ryml_str(value);
}

void YAMLEmitter::emit_array(ryml::NodeRef& node, const Array& array, const Ref<YAMLStyle>& style)
{
  node |= ryml::SEQ;

  if (array.size() == 0) {
    return;
  }

  if (style.is_valid() && style->collection_style == YAMLStyle::COLLECTION_FLOW) {
    node |= ryml::FLOW_SL;
  }

  // Get shared item style if available
  Ref<YAMLStyle> shared_style = style.is_valid() ? style->get_child("_items") : Ref<YAMLStyle>();

  for (int i = 0; i < array.size(); i++) {
    // Check for individual item style, fall back to shared style
    Ref<YAMLStyle> item_style;
    if (style.is_valid()) {
      item_style = style->get_child(String::num_int64(i));
      if (!item_style.is_valid()) {
        item_style = shared_style;
      }
    }
    emit_value(node.append_child(), array[i], item_style);
  }
}

void YAMLEmitter::emit_dictionary(ryml::NodeRef& node, const Dictionary& dict, const Ref<YAMLStyle>& style)
{
  node |= ryml::MAP;

  if (dict.size() == 0) {
    return;
  }

  if (style.is_valid() && style->collection_style == YAMLStyle::MAP_FLOW) {
    node |= ryml::FLOW_SL;
  }

  Array keys = dict.keys();
  for (int i = 0; i < keys.size(); i++) {
    const Variant& key = keys[i];
    ryml::csubstr key_str;

    // Convert key to string efficiently
    if (key.get_type() == Variant::STRING) {
      key_str = to_ryml_str(key);
    } else {
      key_str = to_ryml_str(String(key));
    }

    ryml::NodeRef child = node.append_child();
    child << ryml::key(key_str);

    // Pass child style based on key
    Ref<YAMLStyle> value_style = style.is_valid() ? style->get_child(String(key)) : Ref<YAMLStyle>();
    emit_value(child, dict[key], value_style);
  }
}

void YAMLEmitter::emit_tagged_value(ryml::NodeRef& node, const Variant& value, const Ref<YAMLStyle>& style)
{
  if (!VariantConverterRegistry::has_converter(value.get_type())) {
    UtilityFunctions::push_warning(String("No converter for type " + Variant::get_type_name(value.get_type())));
    return;
  }
  const auto* converter = VariantConverterRegistry::get_converter(value.get_type());
  node.set_val_tag(ryml::to_csubstr(converter->get_full_tag()));
  converter->encode(node, value, style);
}

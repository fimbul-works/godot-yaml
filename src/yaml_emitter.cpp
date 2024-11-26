#include "yaml_emitter.h"
#include "util_numeric.h"
#include "util_string.h"
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void YAMLEmitter::_bind_methods()
{
  ClassDB::bind_method(D_METHOD("emit", "input", "format"), &YAMLEmitter::emit,
          DEFVAL(YAMLFormat::create_default()));
  ClassDB::bind_method(D_METHOD("set_default_format", "format"), &YAMLEmitter::set_default_format);
  ClassDB::bind_method(D_METHOD("get_default_format"), &YAMLEmitter::get_default_format);
}

YAMLEmitter::YAMLEmitter()
{
  default_format = YAMLFormat::create_default();
}

YAMLEmitter::YAMLEmitter(const Ref<YAMLFormat>& format)
{
  default_format = format.is_valid() ? format : YAMLFormat::create_default();
}

YAMLEmitter::~YAMLEmitter() = default;

void YAMLEmitter::set_default_format(const Ref<YAMLFormat>& format)
{
  default_format = format.is_valid() ? format : YAMLFormat::create_default();
}

Ref<YAMLFormat> YAMLEmitter::get_default_format() const
{
  return default_format;
}

Ref<YAMLResult> YAMLEmitter::emit(const Variant& input, const Ref<YAMLFormat>& format)
{
  std::lock_guard<std::mutex> lock(emit_mutex);

  try {
    ryml::Tree tree;
    emit_value(tree.rootref(), input,
            format.is_valid() ? format->get_view() : default_format->get_view());

    std::string yaml_str = ryml::emitrs_yaml<std::string>(tree);
    return YAMLResult::success(String::utf8(yaml_str.c_str(), yaml_str.length()));

  } catch (const std::exception& e) {
    return YAMLResult::error(e.what());
  }
}

void YAMLEmitter::emit_value(ryml::NodeRef& node, const Variant& value, const YAMLFormat::View& format)
{
  // First try to emit as a tagged type if we have a converter
  if (VariantConverterRegistry::has_converter(value.get_type())) {
    if (const auto* converter = VariantConverterRegistry::get_converter(value.get_type())) {
      emit_tagged_value(node, value, format);
      return;
    }
  }

  // Otherwise emit based on variant type
  switch (value.get_type()) {
    case Variant::NIL:
      emit_nil(node);
      break;

    case Variant::BOOL:
      emit_bool(node, value);
      break;

    case Variant::INT:
    case Variant::FLOAT:
      emit_number(node, value);
      break;

    case Variant::STRING:
      emit_string(node, value);
      break;

    case Variant::ARRAY:
      emit_array(node, value, format);
      break;

    case Variant::DICTIONARY:
      emit_dictionary(node, value, format);
      break;

    default:
      String type_name = Variant::get_type_name(value.get_type());
      current_result = YAMLResult::error(vformat("Unsupported type: %s", type_name));
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

void YAMLEmitter::emit_number(ryml::NodeRef& node, const Variant& value)
{
  if (value.get_type() == Variant::INT) {
    node << int_to_string((int64_t)value);
  } else {
    node << float_to_string((double)value);
  }
}

void YAMLEmitter::emit_string(ryml::NodeRef& node, const String& value)
{
  if (value.is_empty()) {
    node << ryml::csubstr {};
    return;
  }

  if (needs_block_style(value)) {
    node |= ryml::BLOCK;
  }
  node << to_ryml_str(value);
}

void YAMLEmitter::emit_array(ryml::NodeRef& node, const Array& array, const YAMLFormat::View& format)
{
  node |= ryml::SEQ;

  if (array.size() == 0) {
    return;
  }

  for (int i = 0; i < array.size(); i++) {
    emit_value(node.append_child(), array[i], format);
  }
}

void YAMLEmitter::emit_dictionary(ryml::NodeRef& node, const Dictionary& dict, const YAMLFormat::View& format)
{
  node |= ryml::MAP;

  if (dict.size() == 0) {
    return;
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
    emit_value(child, dict[key], format);
  }
}

void YAMLEmitter::emit_tagged_value(ryml::NodeRef& node, const Variant& value, const YAMLFormat::View& format)
{
  if (!VariantConverterRegistry::has_converter(value.get_type())) {
    UtilityFunctions::push_warning(String("No converter for type " + Variant::get_type_name(value.get_type())));
    return;
  }
  const auto* converter = VariantConverterRegistry::get_converter(value.get_type());
  node.set_val_tag(ryml::to_csubstr(converter->get_full_tag()));
  converter->encode(node, value, format);
}

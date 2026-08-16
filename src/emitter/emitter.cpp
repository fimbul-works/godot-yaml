#include "emitter.hpp"
#include "../class_registry.hpp"
#include "../util/util_numeric.hpp"
#include "../util/util_string.hpp"

#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <sstream>

using namespace godot;

YAML::Emitter::Emitter() {
	callbacks.m_error = error_callback;
	callbacks.m_user_data = this;
	evt_handler = std::make_unique<ryml::EventHandlerTree>(callbacks);

	init_converters();
}

void YAML::Emitter::init_converters() {
	type_converters = factory.create_converter_set();

	for (const auto &pair : type_converters) {
		if (pair.second) {
			tag_converters[pair.second->get_tag()] = pair.second.get();
		}
	}
}

void YAML::Emitter::error_callback(const char *msg, size_t len, ryml::Location loc, void *user_data) {
	ryml::csubstr error_msg(msg, len);

	// Strip "ERROR: " prefix if present
	const ryml::csubstr strip_error_prefix = "ERROR: ";
	if (error_msg.begins_with(strip_error_prefix)) {
		error_msg = error_msg.sub(strip_error_prefix.len);
	}

	// Only return the first line, which has the relevent error message
	size_t newline_pos = error_msg.find('\n');
	if (newline_pos != ryml::substr::npos) {
		error_msg = error_msg.sub(0, newline_pos);
	}

	auto *emitter = static_cast<Emitter *>(user_data);
	if (emitter) {
		emitter->current_result = YAMLResult::error(from_ryml_str(error_msg), loc.line, loc.col);
		std::longjmp(emitter->jmp_env, 1);
	}

	// Fallback
	throw YAMLException(from_ryml_str(error_msg));
}

ryml::csubstr YAML::Emitter::store_string(const String &str) {
	return string_pool.store(str);
}

YAMLVariantConverter *YAML::Emitter::get_converter_for_type(const Variant::Type type) const {
	auto it = type_converters.find(type);
	return it != type_converters.end() ? it->second.get() : nullptr;
}

YAMLVariantConverter *YAML::Emitter::get_converter_for_tag(const String &tag) const {
	auto it = tag_converters.find(tag);
	return it != tag_converters.end() ? it->second : nullptr;
}

Ref<YAMLResult> YAML::Emitter::emit(const Variant &input, const YAMLStyle::View &style) {
	try {
		current_result = YAMLResult::success(Variant());
		depth = 0;

		if (setjmp(jmp_env) != 0) {
			return current_result;
		}

		emit_value(tree.rootref(), input, style);

		if (current_result->has_error()) {
			return current_result;
		}

		std::string yaml_str = ryml::emitrs_yaml<std::string>(tree);
		return YAMLResult::success(String::utf8(yaml_str.c_str(), yaml_str.length()), nullptr);
	} catch (const YAMLException &e) {
		return YAMLResult::error(e.get_godot_message());
	} catch (const std::exception &e) {
		return YAMLResult::error(e.what());
	} catch (...) {
		return YAMLResult::error("Unknown error occurred during stringification");
	}
}

void YAML::Emitter::emit_value(ryml::NodeRef node, const Variant &value, const YAMLStyle::View &style) {
	depth++;
	check_depth(depth);

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

		case Variant::OBJECT: {
			Object *obj = value.operator Object *();
			if (obj) {
				emit_object(node, obj, style);
			} else {
				emit_nil(node);
			}
			break;
		}

		default: {
			YAMLVariantConverter *converter = get_converter_for_type(value.get_type());
			if (converter) {
				converter->encode(node, value, style);
				node.set_val_tag(converter->get_full_tag());
			} else {
				String type_name = type_str(value);
				String error = vformat("Cannot serialize type: %s", type_name);
				current_result = YAMLResult::error(error);
				std::longjmp(jmp_env, 1);
			}
			break;
		}
	}

	depth--;

	// Add custom tags last
	if ((node.has_val() || node.is_container()) && !node.has_val_tag() && style.is_valid()) {
		if (!style.get_custom_tag().is_empty()) {
			node.set_val_tag(string_pool.store(vformat("!%s", style.get_custom_tag())));
		} else if (style.get_custom_settings().has("tag")) {
			String tag = style.get_custom_settings()["tag"];
			if (!tag.is_empty()) {
				node.set_val_tag(store_string("!" + tag));
			}
		}
	}
}

void YAML::Emitter::emit_nil(ryml::NodeRef &node) {
	ryml::csubstr null = {};
	node << null;
}

void YAML::Emitter::emit_bool(ryml::NodeRef &node, bool value) {
	node << (value ? "true" : "false");
}

void YAML::Emitter::emit_number(ryml::NodeRef &node, const Variant &value, const YAMLStyle::View &style) {
	if (value.get_type() == Variant::INT) {
		YAMLStyle::IntegerFormat format = style.get_integer_format();
		int64_t int_val = value.operator int64_t();
		node << int_to_string(int_val, format);
	} else {
		YAMLStyle::FloatFormat format = style.get_float_format();
		double float_val = value.operator double();
		node << float_to_string(float_val, format);
	}
}

void YAML::Emitter::emit_string(ryml::NodeRef &node, const String &value, const YAMLStyle::View &style) {
	if (style.is_valid()) {
		YAMLStyle::StringStyle string_style = style.get_string_style();

		// Override plain style if the string requires special handling
		if (string_style == YAMLStyle::STRING_PLAIN || string_style == YAMLStyle::STRING_ANY) {
			if (needs_block_style(value)) {
				// Must use block style
				node |= ryml::BLOCK;
				if (value.ends_with("\n") || value.contains("  ")) {
					node |= ryml::VAL_LITERAL;
				} else {
					node |= ryml::VAL_FOLDED;
				}
			} else if (needs_quotes(value)) {
				if (string_style == YAMLStyle::STRING_QUOTE_SINGLE) {
					node |= ryml::VAL_SQUO;
				} else {
					node |= ryml::VAL_DQUO;
				}
			} else {
				// Safe to use plain style
				style.apply_string_style(node);
			}
		} else {
			// User explicitly requested quotes or block style, honor it
			style.apply_string_style(node);
		}
	} else {
		// No style specified, auto-detect
		if (needs_block_style(value)) {
			node |= ryml::BLOCK;
			if (value.ends_with("\n") || value.contains("  ")) {
				node |= ryml::VAL_LITERAL;
			} else {
				node |= ryml::VAL_FOLDED;
			}
		} else if (needs_quotes(value)) {
			// Must quote, default to double quotes
			node |= ryml::VAL_DQUO;
		}
	}

	const CharString utf8 = value.utf8();
	node << c4::csubstr(utf8.get_data(), utf8.length());
}

void YAML::Emitter::emit_array(ryml::NodeRef &node, const Array &array, const YAMLStyle::View &style) {
	node |= ryml::SEQ;

	style.apply_flow_style(node);

	if (array.size() == 0) {
		return;
	}

	YAMLStyle::View template_style = style.get_template_style();

	for (int64_t i = 0; i < array.size(); i++) {
		// Check for individual item style
		YAMLStyle::View item_style;
		if (style.is_valid()) {
			item_style = style.get_child(String::num_int64(i));

			// Fall back to template
			if (!item_style.is_valid()) {
				item_style = template_style;
			}
		}

		ryml::NodeRef child = node.append_child();
		emit_value(child, array[i], item_style);
	}
}

void YAML::Emitter::emit_dictionary(ryml::NodeRef &node, const Dictionary &dict, const YAMLStyle::View &style) {
	node |= ryml::MAP;

	style.apply_flow_style(node);

	Array keys = dict.keys();

	for (int64_t i = 0; i < keys.size(); i++) {
		ryml::NodeRef child = node.append_child();
		ryml::csubstr key_str = store_string(keys[i]);
		child << ryml::key(key_str);

		YAMLStyle::View child_style = style.is_valid() ? style.get_child(String(keys[i])) : YAMLStyle::View();
		emit_value(child, dict[keys[i]], child_style);
	}
}

void YAML::Emitter::emit_object(ryml::NodeRef &node, Object *obj, const YAMLStyle::View &style) {
	String class_name = obj->get_class();

	// Handle custom classes
	Ref<Script> script = obj->get_script();
	if (script.is_valid() && !script->get_global_name().is_empty()) {
		class_name = script->get_global_name();

		if (YAMLClassRegistry::get_singleton().has_class(class_name)) {
			YAMLClassRegistry::ClassInfo class_info = YAMLClassRegistry::get_singleton().get_class_info(class_name);

			if (class_info.script_class.is_valid()) {
				const String tag = class_info.tag.is_empty() ? class_name : class_info.tag;
				const StringName serialize = class_info.serialize_method;
				Variant data = obj->call(serialize);
				if (data) {
					emit_value(node, data, style);
					node.set_val_tag(store_string("!" + tag));
					return;
				}
			}
		}
	}

	const Resource *res = Object::cast_to<const Resource>(obj);
	if (res) {
		emit_resource(node, res, style);
		return;
	}

	// FIXME: Handle Object types
	String error = vformat("Cannot stringify Object of type: %s", class_name);
	current_result = YAMLResult::error(error);
	std::longjmp(jmp_env, 1);
}

void YAML::Emitter::emit_resource(ryml::NodeRef &node, const Resource *res, const YAMLStyle::View &style) {
	String path = res->get_path();
	bool is_local = res->is_local_to_scene();

	if (path.is_empty()) {
		String error = "Cannot serialize Resource without path";
		current_result = YAMLResult::error(error);
		std::longjmp(jmp_env, 1);
	}

	if (is_local || path.to_lower().contains("::")) {
		String error = "Cannot serialize local Resource";
		current_result = YAMLResult::error(error);
		std::longjmp(jmp_env, 1);
	}

	emit_string(node, path, YAMLStyle::View()); // Use default string style
	node.set_val_tag("!Resource");
}

void YAML::Emitter::check_depth(int current_depth) {
	if (current_depth > MAX_DEPTH) {
		String error = vformat("Maximum nesting depth exceeded (%d). Possible circular reference?", MAX_DEPTH);
		current_result = YAMLResult::error(error);
		std::longjmp(jmp_env, 1);
	}
}

#include "node_path_variant_converter.hpp"
#include "../exception.hpp"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void YAMLNodePathVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const NodePath path = v.operator NodePath();

	style.apply_string_style(node);
	node << to_ryml_str(String(path));
}

Variant YAMLNodePathVariantConverter::decode(const ryml::ConstNodeRef &node, YAMLParserContext *context) const {
	try {
		if (!node.has_val()) {
			throw create_invalid_format_exception(node);
		}

		if (node.val_is_null()) {
			return NodePath(); // Return empty NodePath
		}

		if (context->detect_style) {
			YAMLStyle::detect_string_style(node, context->current_style());
		}

		return NodePath(from_ryml_str(node.val()));
	} catch (const YAMLException &) {
		throw; // Re-throw YAML exceptions
	} catch (const std::exception &e) {
		throw create_decode_error_exception(e.what(), node);
	}
}

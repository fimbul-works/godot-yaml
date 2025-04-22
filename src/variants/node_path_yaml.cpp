#include "node_path_yaml.h"
#include "../exception.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void NodePathVariantConverter::encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const {
	const NodePath path = v.operator NodePath();
	const String str = String(path);
	if (str.is_empty()) {
		node << ryml::csubstr{};
	} else {
		if (style.is_valid()) {
			style.apply_scalar_style(node);
			style.apply_quote_style(node);
		} else {
			node |= ryml::VAL_DQUO;
		}
		node << to_ryml_str(str);
	}
}

Variant NodePathVariantConverter::decode(const ryml::ConstNodeRef &node) const {
	try {
		if (node.val_is_null()) {
			return NodePath(); // Return empty NodePath
		}

		if (!node.has_val()) {
			throw YAMLException::create_invalid_format("NodePath");
		}

		return NodePath(from_ryml_str(node.val()));
	} catch (const YAMLException &) {
		throw; // Re-throw YAML exceptions
	} catch (const std::exception &e) {
		throw YAMLException::create_decode_error("NodePath", e.what());
	}
}

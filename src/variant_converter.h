#ifndef VARIANT_CONVERTER_H
#define VARIANT_CONVERTER_H

#include "exception.h"
#include "string_pool.h"
#include "style_view.h"
#include "yaml.h"

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <ryml.hpp>

#define DEFINE_YAML_TAG(TAG_VALUE, VARIANT_TYPE)           \
	static constexpr const char *TAG = TAG_VALUE;          \
	static constexpr const char *FULL_TAG = "!" TAG_VALUE; \
	const char *get_tag() const override {                 \
		return TAG;                                        \
	}                                                      \
	const char *get_full_tag() const override {            \
		return FULL_TAG;                                   \
	}                                                      \
	const Variant::Type get_type() const override {        \
		return VARIANT_TYPE;                               \
	}

namespace godot {

// Forward declaration
class ConverterFactory;

class VariantConverter {
public:
	explicit VariantConverter() = default;
	virtual ~VariantConverter() = default;

	// Tag identification
	virtual const char *get_tag() const = 0;
	virtual const char *get_full_tag() const = 0;
	virtual const Variant::Type get_type() const = 0;

	// Pure virtual encode method that derived classes must implement
	virtual void encode(ryml::NodeRef &node, const Variant &v, const YAMLStyle::View &style) const = 0;

	// Pure virtual decode method
	virtual Variant decode(const ryml::ConstNodeRef &node) const = 0;

	// Set/get the parser reference
	void set_parser(const ryml::Parser *parser) { m_parser = parser; }
	const ryml::Parser *get_parser() const { return m_parser; }

protected:
	mutable YAMLStringPool string_pool;
	const ryml::Parser *m_parser = nullptr;

	ryml::csubstr store_string(const String &str) const {
		return string_pool.store(str);
	}

	inline YAMLException create_exception(const String &message, const ryml::ConstNodeRef &node) const {
		if (m_parser && node.valid()) {
			try {
				ryml::Location loc = m_parser->location(node);
				return YAMLException(message, loc);
			} catch (...) {
				// If location can't be determined, fall back to basic message
			}
		}
		return YAMLException(message);
	}

	inline YAMLException create_invalid_format_exception(const char *type_name, const ryml::ConstNodeRef &node) const {
		return create_exception(vformat("Invalid %s format", type_name), node);
	}

	inline YAMLException create_invalid_sequence_length_exception(const char *type_name, int expected_length, const ryml::ConstNodeRef &node) const {
		return create_exception(vformat("%s sequence must have %d elements", type_name, expected_length), node);
	}

	inline YAMLException create_decode_error_exception(const char *type_name, const char *details, const ryml::ConstNodeRef &node) const {
		return create_exception(vformat("Failed to decode %s: %s", type_name, details), node);
	}

	inline void check_required_fields(const ryml::ConstNodeRef &node, const std::vector<const char *> &required_fields) const {
		std::vector<String> missing_fields;

		for (const char *field : required_fields) {
			if (!node.has_child(field)) {
				missing_fields.push_back(String(field));
			}
		}

		if (!missing_fields.empty()) {
			String missing_list;
			for (size_t i = 0; i < missing_fields.size(); i++) {
				missing_list += missing_fields[i];
				if (i < missing_fields.size() - 1) {
					missing_list += "', '";
				}
			}

			throw create_exception(String(get_tag()) + " missing required field" + (missing_fields.size() > 1 ? "s" : "") + ": '" + missing_list + "'", node);
		}
	}

	inline const bool is_hex_char(char c) const {
		return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
	}

	inline const bool is_whitespace_char(char c) const {
		return c == ' ' || c == '\t' || c == '\n' || c == '\r';
	}
};

} // namespace godot

#endif // VARIANT_CONVERTER_H

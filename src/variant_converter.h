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

protected:
	mutable YAMLStringPool string_pool;

	ryml::csubstr store_string(const String &str) const {
		return string_pool.store(str);
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

			throw YAMLException(String(get_tag()) + " missing required field" + (missing_fields.size() > 1 ? "s" : "") + ": '" + missing_list + "'");
		}
	}
};

} // namespace godot

#endif // VARIANT_CONVERTER_H

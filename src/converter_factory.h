/**
 * @file converter_factory.h
 * @brief Defines the ConverterFactory class for managing type converters.
 *
 * This file contains the ConverterFactory class which creates and manages
 * the various type converters used in YAML parsing and emission. It provides
 * a central registry for all supported type conversions between YAML and
 * Godot types.
 */

#ifndef CONVERTER_FACTORY_H
#define CONVERTER_FACTORY_H

#include "string_hash.h"
#include "variant_converter.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>

#include <functional>
#include <memory>
#include <unordered_map>

namespace godot {

/**
 * @class ConverterFactory
 * @brief Factory for creating and managing type converters.
 *
 * The ConverterFactory class serves as a registry and factory for
 * VariantConverter instances. It manages the mapping between Godot Variant
 * types, YAML tags, and their corresponding converter implementations.
 *
 * It provides methods to:
 * - Create individual converters by type or tag
 * - Create complete sets of converters
 * - Register new converter types
 *
 * This centralized approach allows for easy extension with new types
 * and ensures consistent type handling throughout the YAML module.
 */
class ConverterFactory {
public:
	/**
	 * @brief Constructs a new ConverterFactory.
	 *
	 * Initializes with all built-in type converters registered.
	 */
	ConverterFactory();

	/**
	 * @brief Default destructor.
	 */
	~ConverterFactory() = default;

	/**
	 * @brief Non-copyable class to prevent accidental sharing.
	 */
	ConverterFactory(const ConverterFactory &) = delete;
	ConverterFactory &operator=(const ConverterFactory &) = delete;

	/**
	 * @brief Creates a converter for a specific Variant type.
	 *
	 * @param type The Variant type
	 * @return std::unique_ptr<VariantConverter> The created converter or nullptr if not supported
	 */
	std::unique_ptr<VariantConverter> create_converter(Variant::Type type);

	/**
	 * @brief Creates and casts a converter to a specific derived type.
	 *
	 * This template function creates a converter for a specific Variant type
	 * and casts it to the requested derived converter type if compatible.
	 *
	 * @tparam T The derived converter type to cast to
	 * @param type The Variant type
	 * @return std::unique_ptr<T> The created and cast converter or nullptr if invalid
	 */
	template <typename T>
	std::unique_ptr<T> create_converter_as(Variant::Type type) {
		auto base = create_converter(type);
		if (!base) {
			return nullptr;
		}

		T *derived = dynamic_cast<T *>(base.release());
		if (!derived) {
			// Handle error case - wrong type
			return nullptr;
		}

		return std::unique_ptr<T>(derived);
	}

	/**
	 * @brief Creates a converter for a specific YAML tag.
	 *
	 * @param tag The YAML tag
	 * @return std::unique_ptr<VariantConverter> The created converter or nullptr if not supported
	 */
	std::unique_ptr<VariantConverter> create_converter_for_tag(const String &tag);

	/**
	 * @brief Creates a complete set of converters for all supported types.
	 *
	 * @return std::unordered_map<Variant::Type, std::unique_ptr<VariantConverter>>
	 *         Map of Variant types to their converters
	 */
	std::unordered_map<Variant::Type, std::unique_ptr<VariantConverter>> create_converter_set();

private:
	/**
	 * @brief Gets the Variant type for a specific YAML tag.
	 *
	 * @param tag The YAML tag
	 * @return Variant::Type The corresponding Variant type or NIL if not found
	 */
	Variant::Type get_type_for_tag(const String &tag);

	/**
	 * @brief Type information for registering converters.
	 */
	struct ConverterInfo {
		Variant::Type type; ///< The Variant type
		String tag; ///< The YAML tag
		std::function<std::unique_ptr<VariantConverter>(ConverterFactory *)> factory_func; ///< Factory function
	};

	/**
	 * @brief Maps from type to converter info.
	 */
	std::unordered_map<Variant::Type, ConverterInfo> type_map;

	/**
	 * @brief Maps from tag to variant type.
	 */
	std::unordered_map<String, Variant::Type> tag_map;

	/**
	 * @brief Helper to register a converter type.
	 *
	 * This template function registers a converter class for a specific
	 * Variant type and YAML tag. It creates a factory function that
	 * instantiates the converter when needed.
	 *
	 * @tparam T The converter class type
	 * @param type The Variant type
	 * @param tag The YAML tag
	 */
	template <typename T>
	void register_converter(Variant::Type type, const String &tag) {
		ConverterInfo info{
			type,
			tag,
			[](ConverterFactory *factory) -> std::unique_ptr<VariantConverter> {
				if constexpr (std::is_constructible_v<T, ConverterFactory *>) {
					return std::make_unique<T>(factory);
				} else {
					return std::make_unique<T>();
				}
			}
		};

		type_map[type] = info;
		tag_map[tag] = type;
	}
};

} // namespace godot

#endif // CONVERTER_FACTORY_H

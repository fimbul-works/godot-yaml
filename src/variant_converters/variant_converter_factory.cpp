#include "variant_converter_factory.hpp"

#include "aabb_variant_converter.hpp"
#include "basis_variant_converter.hpp"
#include "color_variant_converter.hpp"
#include "node_path_variant_converter.hpp"
#include "packed_byte_array_variant_converter.hpp"
#include "packed_color_array_variant_converter.hpp"
#include "packed_float32_array_variant_converter.hpp"
#include "packed_float64_array_variant_converter.hpp"
#include "packed_int32_array_variant_converter.hpp"
#include "packed_int64_array_variant_converter.hpp"
#include "packed_string_array_variant_converter.hpp"
#include "packed_vector2_array_variant_converter.hpp"
#include "packed_vector3_array_variant_converter.hpp"
#include "plane_variant_converter.hpp"
#include "projection_variant_converter.hpp"
#include "quaternion_variant_converter.hpp"
#include "rect2_variant_converter.hpp"
#include "rect2i_variant_converter.hpp"
#include "string_name_variant_converter.hpp"
#include "transform2d_variant_converter.hpp"
#include "transform3d_variant_converter.hpp"
#include "vector2_variant_converter.hpp"
#include "vector2i_variant_converter.hpp"
#include "vector3_variant_converter.hpp"
#include "vector3i_variant_converter.hpp"
#include "vector4_variant_converter.hpp"
#include "vector4i_variant_converter.hpp"

using namespace godot;

YAMLVariantConverterFactory::YAMLVariantConverterFactory() {
	register_converter<YAMLAABBVariantConverter>(Variant::AABB, YAMLAABBVariantConverter::TAG);
	register_converter<YAMLBasisVariantConverter>(Variant::BASIS, YAMLBasisVariantConverter::TAG);
	register_converter<YAMLColorVariantConverter>(Variant::COLOR, YAMLColorVariantConverter::TAG);
	register_converter<YAMLNodePathVariantConverter>(Variant::NODE_PATH, YAMLNodePathVariantConverter::TAG);
	register_converter<YAMLPackedByteArrayVariantConverter>(Variant::PACKED_BYTE_ARRAY, YAMLPackedByteArrayVariantConverter::TAG);
	register_converter<YAMLPackedColorArrayVariantConverter>(Variant::PACKED_COLOR_ARRAY, YAMLPackedColorArrayVariantConverter::TAG);
	register_converter<YAMLPackedFloat32ArrayVariantConverter>(Variant::PACKED_FLOAT32_ARRAY, YAMLPackedFloat32ArrayVariantConverter::TAG);
	register_converter<YAMLPackedFloat64ArrayVariantConverter>(Variant::PACKED_FLOAT64_ARRAY, YAMLPackedFloat64ArrayVariantConverter::TAG);
	register_converter<YAMLPackedInt32ArrayVariantConverter>(Variant::PACKED_INT32_ARRAY, YAMLPackedInt32ArrayVariantConverter::TAG);
	register_converter<YAMLPackedInt64ArrayVariantConverter>(Variant::PACKED_INT64_ARRAY, YAMLPackedInt64ArrayVariantConverter::TAG);
	register_converter<YAMLPackedStringArrayVariantConverter>(Variant::PACKED_STRING_ARRAY, YAMLPackedStringArrayVariantConverter::TAG);
	register_converter<YAMLPackedVector2ArrayVariantConverter>(Variant::PACKED_VECTOR2_ARRAY, YAMLPackedVector2ArrayVariantConverter::TAG);
	register_converter<YAMLPackedVector3ArrayVariantConverter>(Variant::PACKED_VECTOR3_ARRAY, YAMLPackedVector3ArrayVariantConverter::TAG);
	register_converter<YAMLPlaneVariantConverter>(Variant::PLANE, YAMLPlaneVariantConverter::TAG);
	register_converter<YAMLProjectionVariantConverter>(Variant::PROJECTION, YAMLProjectionVariantConverter::TAG);
	register_converter<YAMLQuaternionVariantConverter>(Variant::QUATERNION, YAMLQuaternionVariantConverter::TAG);
	register_converter<YAMLRect2VariantConverter>(Variant::RECT2, YAMLRect2VariantConverter::TAG);
	register_converter<YAMLRect2iVariantConverter>(Variant::RECT2I, YAMLRect2iVariantConverter::TAG);
	register_converter<YAMLStringNameVariantConverter>(Variant::STRING_NAME, YAMLStringNameVariantConverter::TAG);
	register_converter<YAMLTransform2DVariantConverter>(Variant::TRANSFORM2D, YAMLTransform2DVariantConverter::TAG);
	register_converter<YAMLTransform3DVariantConverter>(Variant::TRANSFORM3D, YAMLTransform3DVariantConverter::TAG);
	register_converter<YAMLVector2VariantConverter>(Variant::VECTOR2, YAMLVector2VariantConverter::TAG);
	register_converter<YAMLVector2iVariantConverter>(Variant::VECTOR2I, YAMLVector2iVariantConverter::TAG);
	register_converter<YAMLVector3VariantConverter>(Variant::VECTOR3, YAMLVector3VariantConverter::TAG);
	register_converter<YAMLVector3iVariantConverter>(Variant::VECTOR3I, YAMLVector3iVariantConverter::TAG);
	register_converter<YAMLVector4VariantConverter>(Variant::VECTOR4, YAMLVector4VariantConverter::TAG);
	register_converter<YAMLVector4iVariantConverter>(Variant::VECTOR4I, YAMLVector4iVariantConverter::TAG);
}

std::unique_ptr<YAMLVariantConverter> YAMLVariantConverterFactory::create_converter(Variant::Type type) {
	auto it = type_map.find(type);
	if (it == type_map.end()) {
		return nullptr;
	}

	return it->second.factory_func(this);
}

std::unique_ptr<YAMLVariantConverter> YAMLVariantConverterFactory::create_converter_for_tag(const String &tag) {
	return create_converter(get_type_for_tag(tag));
}

std::unordered_map<Variant::Type, std::unique_ptr<YAMLVariantConverter>> YAMLVariantConverterFactory::create_converter_set() {
	std::unordered_map<Variant::Type, std::unique_ptr<YAMLVariantConverter>> converters;

	for (const auto &[type, info] : type_map) {
		converters[type] = info.factory_func(this);
	}

	return converters;
}

Variant::Type YAMLVariantConverterFactory::get_type_for_tag(const String &tag) {
	auto it = tag_map.find(tag);
	if (it == tag_map.end()) {
		return Variant::NIL; // Not found
	}

	return it->second;
}

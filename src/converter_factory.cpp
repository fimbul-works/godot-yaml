#include "converter_factory.h"

#include "variants/aabb_yaml.h"
#include "variants/basis_yaml.h"
#include "variants/color_yaml.h"
#include "variants/node_path_yaml.h"
#include "variants/packed_byte_array_yaml.h"
#include "variants/packed_color_array_yaml.h"
#include "variants/packed_float32_array_yaml.h"
#include "variants/packed_float64_array_yaml.h"
#include "variants/packed_int32_array_yaml.h"
#include "variants/packed_int64_array_yaml.h"
#include "variants/packed_string_array_yaml.h"
#include "variants/packed_vector2_array_yaml.h"
#include "variants/packed_vector3_array_yaml.h"
#include "variants/plane_yaml.h"
#include "variants/projection_yaml.h"
#include "variants/quaternion_yaml.h"
#include "variants/rect2_yaml.h"
#include "variants/rect2i_yaml.h"
#include "variants/string_name_yaml.h"
#include "variants/transform2d_yaml.h"
#include "variants/transform3d_yaml.h"
#include "variants/vector2_yaml.h"
#include "variants/vector2i_yaml.h"
#include "variants/vector3_yaml.h"
#include "variants/vector3i_yaml.h"
#include "variants/vector4_yaml.h"
#include "variants/vector4i_yaml.h"

using namespace godot;

ConverterFactory::ConverterFactory() {
	register_converter<AABBVariantConverter>(Variant::AABB, AABBVariantConverter::TAG);
	register_converter<BasisVariantConverter>(Variant::BASIS, BasisVariantConverter::TAG);
	register_converter<ColorVariantConverter>(Variant::COLOR, ColorVariantConverter::TAG);
	register_converter<NodePathVariantConverter>(Variant::NODE_PATH, NodePathVariantConverter::TAG);
	register_converter<PackedByteArrayVariantConverter>(Variant::PACKED_BYTE_ARRAY, PackedByteArrayVariantConverter::TAG);
	register_converter<PackedColorArrayVariantConverter>(Variant::PACKED_COLOR_ARRAY, PackedColorArrayVariantConverter::TAG);
	register_converter<PackedFloat32ArrayVariantConverter>(Variant::PACKED_FLOAT32_ARRAY, PackedFloat32ArrayVariantConverter::TAG);
	register_converter<PackedFloat64ArrayVariantConverter>(Variant::PACKED_FLOAT64_ARRAY, PackedFloat64ArrayVariantConverter::TAG);
	register_converter<PackedInt32ArrayVariantConverter>(Variant::PACKED_INT32_ARRAY, PackedInt32ArrayVariantConverter::TAG);
	register_converter<PackedInt64ArrayVariantConverter>(Variant::PACKED_INT64_ARRAY, PackedInt64ArrayVariantConverter::TAG);
	register_converter<PackedStringArrayVariantConverter>(Variant::PACKED_STRING_ARRAY, PackedStringArrayVariantConverter::TAG);
	register_converter<PackedVector2ArrayVariantConverter>(Variant::PACKED_VECTOR2_ARRAY, PackedVector2ArrayVariantConverter::TAG);
	register_converter<PackedVector3ArrayVariantConverter>(Variant::PACKED_VECTOR3_ARRAY, PackedVector3ArrayVariantConverter::TAG);
	register_converter<PlaneVariantConverter>(Variant::PLANE, PlaneVariantConverter::TAG);
	register_converter<ProjectionVariantConverter>(Variant::PROJECTION, ProjectionVariantConverter::TAG);
	register_converter<QuaternionVariantConverter>(Variant::QUATERNION, QuaternionVariantConverter::TAG);
	register_converter<Rect2VariantConverter>(Variant::RECT2, Rect2VariantConverter::TAG);
	register_converter<Rect2iVariantConverter>(Variant::RECT2I, Rect2iVariantConverter::TAG);
	register_converter<StringNameVariantConverter>(Variant::STRING_NAME, StringNameVariantConverter::TAG);
	register_converter<Transform2DVariantConverter>(Variant::TRANSFORM2D, Transform2DVariantConverter::TAG);
	register_converter<Transform3DVariantConverter>(Variant::TRANSFORM3D, Transform3DVariantConverter::TAG);
	register_converter<Vector2VariantConverter>(Variant::VECTOR2, Vector2VariantConverter::TAG);
	register_converter<Vector2iVariantConverter>(Variant::VECTOR2I, Vector2iVariantConverter::TAG);
	register_converter<Vector3VariantConverter>(Variant::VECTOR3, Vector3VariantConverter::TAG);
	register_converter<Vector3iVariantConverter>(Variant::VECTOR3I, Vector3iVariantConverter::TAG);
	register_converter<Vector4VariantConverter>(Variant::VECTOR4, Vector4VariantConverter::TAG);
	register_converter<Vector4iVariantConverter>(Variant::VECTOR4I, Vector4iVariantConverter::TAG);
}

std::unique_ptr<VariantConverter> ConverterFactory::create_converter(Variant::Type type) {
	auto it = type_map.find(type);
	if (it == type_map.end()) {
		return nullptr;
	}

	return it->second.factory_func(this);
}

std::unique_ptr<VariantConverter> ConverterFactory::create_converter_for_tag(const String &tag) {
	return create_converter(get_type_for_tag(tag));
}

std::unordered_map<Variant::Type, std::unique_ptr<VariantConverter>> ConverterFactory::create_converter_set() {
	std::unordered_map<Variant::Type, std::unique_ptr<VariantConverter>> converters;

	for (const auto &[type, info] : type_map) {
		converters[type] = info.factory_func(this);
	}

	return converters;
}

Variant::Type ConverterFactory::get_type_for_tag(const String &tag) {
	auto it = tag_map.find(tag);
	if (it == tag_map.end()) {
		return Variant::NIL; // Not found
	}

	return it->second;
}

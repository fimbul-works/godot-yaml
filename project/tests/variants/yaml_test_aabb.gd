extends YAMLTest
## Test suite for AABB YAML serialization and styling

# Test values with different complexity
var test_values = {
	"Simple": AABB(Vector3(1, 2, 3), Vector3(4, 5, 6)),
	"Zero": AABB(Vector3.ZERO, Vector3.ZERO),
	"Negative": AABB(Vector3(-1, -2, -3), Vector3(4, 5, 6)),
	"Large": AABB(Vector3(1000, 2000, 3000), Vector3(4000, 5000, 6000)),
	"Decimal": AABB(Vector3(1.5, 2.5, 3.5), Vector3(4.5, 5.5, 6.5)),
}

func _init():
	test_name = "AABB"

## Test basic serialization/deserialization without styles
func test_basic_serialization() -> void:
	run_basic_serialization_test(test_values, is_aabb_equal)

## Test different container forms (map vs sequence)
func test_container_forms() -> void:
	var aabb = AABB(Vector3(1, 2, 3), Vector3(4, 5, 6))
	run_container_forms_test(aabb, is_aabb_equal, ["position", "size"])

## Test different flow styles (block vs flow)
func test_flow_styles() -> void:
	var aabb = AABB(Vector3(1, 2, 3), Vector3(4, 5, 6))
	run_flow_styles_test(aabb, is_aabb_equal)

## Test nested styles for position and size components
func test_nested_styles() -> void:
	var aabb = AABB(Vector3(1, 2, 3), Vector3(4, 5, 6))

	# Create parent style
	var parent_style = YAML.create_style()

	# Create different styles for position and size
	parent_style.set_child("position", YAML.create_style().set_flow_style(YAMLStyle.FLOW_SINGLE))
	parent_style.set_child("size", YAML.create_style().set_flow_style(YAMLStyle.FLOW_NONE))

	var result = YAML.stringify(aabb, parent_style)

	assert_stringify_success(result, "nested styles")
	if not result.has_error():
		print_rich("• Nested styles (position=flow, size=block):")
		print_rich(result.get_data())

		# Verify position has flow style and size has block style
		assert_yaml_has_feature(result.get_data(), "position: {", "Position has flow style")
		assert_true(
			result.get_data().find("size:") != -1 && result.get_data().find("size: {") == -1,
			"Size has block style"
		)

	# Test roundtrip
	assert_roundtrip(YAML.parse(result.get_data()), aabb, is_aabb_equal, "nested styles")

## Test roundtrip conversion with style detection enabled
func test_roundtrip_with_styles() -> void:
	var aabb = AABB(Vector3(1, 2, 3), Vector3(4, 5, 6))

	run_style_roundtrip_test(
		aabb,
		is_aabb_equal,
		func(parsed_aabb): return AABB(parsed_aabb.position + Vector3(10, 10, 10), parsed_aabb.size)
	)

## Test error handling for invalid YAML
func test_parsing_errors() -> void:
	# Test AABB-specific parsing errors
	var invalid_aabb_yaml = """
!AABB
position: {x: 1, y: 2, z: 3}
# Missing size field
"""
	assert_parse_error(invalid_aabb_yaml, "Missing required field detection")

	var wrong_type_yaml = """
!AABB [1, 2, 3]
"""
	assert_parse_error(wrong_type_yaml, "Wrong type/format detection")

	var invalid_sequence_length = """
!AABB
- {x: 1, y: 2, z: 3}
- {x: 4, y: 5, z: 6}
- {x: 7, y: 8, z: 9}
"""
	assert_parse_error(invalid_sequence_length, "Invalid sequence length detection")

## Helper function to check if AABBs are equal (with floating point precision)
func is_aabb_equal(a: AABB, b: AABB) -> bool:
	return a.position.is_equal_approx(b.position) && a.size.is_equal_approx(b.size)

extends YAMLTest
## Test suite for Vector4 YAML serialization and styling

# Test values with different vector configurations
var test_values = {
	"Zero": Vector4.ZERO,
	"One": Vector4.ONE,
	"Simple": Vector4(10, 20, 30, 40),
	"Negative": Vector4(-50, -60, -70, -80),
	"Mixed": Vector4(90, -100, 110, -120),
	"Decimal": Vector4(1.5, 2.75, 3.25, 4.125),
	"Large": Vector4(1000, 2000, 3000, 4000),
	"Very_small": Vector4(0.0001, 0.0002, 0.0003, 0.0004),
	"Normalized": Vector4(2, 3, 4, 5).normalized(),
	"Homogeneous": Vector4(5, 10, 15, 1),  # W=1 for position in homogeneous coords
	"Direction": Vector4(5, 10, 15, 0)     # W=0 for direction in homogeneous coords
}

func _init():
	test_name = "Vector4"

## Test basic serialization/deserialization without styles
func test_basic_serialization() -> void:
	run_basic_serialization_test(test_values, is_vector4_equal)

## Test different container forms (map vs sequence)
func test_container_forms() -> void:
	var vec = Vector4(10, 20, 30, 40)
	run_container_forms_test(vec, is_vector4_equal, ["x", "y", "z", "w"])

## Test different flow styles (block vs flow)
func test_flow_styles() -> void:
	var vec = Vector4(10, 20, 30, 40)
	run_flow_styles_test(vec, is_vector4_equal)

## Test roundtrip conversion with style detection enabled
func test_roundtrip_with_styles() -> void:
	var vec = Vector4(10, 20, 30, 40)

	run_style_roundtrip_test(
		vec,
		is_vector4_equal,
		func(parsed_vec): return parsed_vec * 2
	)

## Test error handling for invalid YAML
func test_parsing_errors() -> void:
	# Test Vector4-specific parsing errors
	var invalid_vector4_yaml = """
!Vector4
x: 1.0
y: 2.0
z: 3.0
# Missing w field
"""
	assert_parse_error(invalid_vector4_yaml, "Missing required field detection")

	var wrong_type_yaml = """
!Vector4 "not a vector4"
"""
	assert_parse_error(wrong_type_yaml, "Wrong type/format detection")

	var invalid_sequence_length = """
!Vector4
- 1.0
- 2.0
- 3.0
"""
	assert_parse_error(invalid_sequence_length, "Invalid sequence length detection")

## Test special values (INF, NAN)
func test_special_values() -> void:
	var special_values = {
		"Infinity": Vector4(INF, INF, INF, INF),
		"Negative Infinity": Vector4(-INF, -INF, -INF, -INF),
		"Mixed Infinity": Vector4(INF, -INF, INF, -INF),
		"NaN": Vector4(NAN, NAN, NAN, NAN)
	}

	for name in special_values:
		var vec = special_values[name]
		var result = YAML.stringify(vec)

		assert_stringify_success(result, "Special value: " + name)
		if result.has_error():
			continue

		print_rich("• %s: %s" % [name, result.get_data()])

		# Parse back and check (skip comparison for NaN)
		var parse_result = YAML.parse(result.get_data())
		assert_parse_success(parse_result, "Parse special value: " + name)

		if !parse_result.has_error():
			var parsed_vec = parse_result.get_data()

			# Check INF values are preserved
			if name == "Infinity":
				assert_true(is_inf(parsed_vec.x) && parsed_vec.x > 0, "Positive infinity preserved for x")
				assert_true(is_inf(parsed_vec.y) && parsed_vec.y > 0, "Positive infinity preserved for y")
				assert_true(is_inf(parsed_vec.z) && parsed_vec.z > 0, "Positive infinity preserved for z")
				assert_true(is_inf(parsed_vec.w) && parsed_vec.w > 0, "Positive infinity preserved for w")
			elif name == "Negative Infinity":
				assert_true(is_inf(parsed_vec.x) && parsed_vec.x < 0, "Negative infinity preserved for x")
				assert_true(is_inf(parsed_vec.y) && parsed_vec.y < 0, "Negative infinity preserved for y")
				assert_true(is_inf(parsed_vec.z) && parsed_vec.z < 0, "Negative infinity preserved for z")
				assert_true(is_inf(parsed_vec.w) && parsed_vec.w < 0, "Negative infinity preserved for w")
			elif name == "Mixed Infinity":
				assert_true(is_inf(parsed_vec.x) && parsed_vec.x > 0, "Positive infinity preserved for x")
				assert_true(is_inf(parsed_vec.y) && parsed_vec.y < 0, "Negative infinity preserved for y")
				assert_true(is_inf(parsed_vec.z) && parsed_vec.z > 0, "Positive infinity preserved for z")
				assert_true(is_inf(parsed_vec.w) && parsed_vec.w < 0, "Negative infinity preserved for w")
			elif name == "NaN":
				assert_true(is_nan(parsed_vec.x), "NaN preserved for x")
				assert_true(is_nan(parsed_vec.y), "NaN preserved for y")
				assert_true(is_nan(parsed_vec.z), "NaN preserved for z")
				assert_true(is_nan(parsed_vec.w), "NaN preserved for w")

## Test homogeneous coordinates conventions
func test_homogeneous_coords() -> void:
	# Test handling of homogeneous coordinates
	var position = Vector4(10, 20, 30, 1)  # w=1 represents a position
	var direction = Vector4(10, 20, 30, 0) # w=0 represents a direction

	# Serialize and deserialize position
	var pos_result = YAML.stringify(position)
	assert_stringify_success(pos_result, "Homogeneous position")

	if !pos_result.has_error():
		var parse_result = YAML.parse(pos_result.get_data())
		assert_parse_success(parse_result, "Parse homogeneous position")

		if !parse_result.has_error():
			var parsed_pos = parse_result.get_data()
			assert_equal(parsed_pos.w, 1.0, "Homogeneous position w=1 preserved")

	# Serialize and deserialize direction
	var dir_result = YAML.stringify(direction)
	assert_stringify_success(dir_result, "Homogeneous direction")

	if !dir_result.has_error():
		var parse_result = YAML.parse(dir_result.get_data())
		assert_parse_success(parse_result, "Parse homogeneous direction")

		if !parse_result.has_error():
			var parsed_dir = parse_result.get_data()
			assert_equal(parsed_dir.w, 0.0, "Homogeneous direction w=0 preserved")

## Helper function to check if Vector4 instances are equal (with floating point precision)
func is_vector4_equal(a: Vector4, b: Vector4, epsilon: float = 0.00001) -> bool:
	return a.is_equal_approx(b)

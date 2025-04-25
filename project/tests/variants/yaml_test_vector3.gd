extends YAMLTest
## Test suite for Vector3 YAML serialization and styling

# Test values with different vector configurations
var test_values = {
	"Zero": Vector3.ZERO,
	"One": Vector3.ONE,
	"Up": Vector3.UP,
	"Down": Vector3.DOWN,
	"Left": Vector3.LEFT,
	"Right": Vector3.RIGHT,
	"Forward": Vector3.FORWARD,
	"Back": Vector3.BACK,
	"Simple": Vector3(10, 20, 30),
	"Negative": Vector3(-40, -50, -60),
	"Mixed": Vector3(70, -80, 90),
	"Decimal": Vector3(1.5, 2.75, 3.25),
	"Large": Vector3(1000, 2000, 3000),
	"Very_small": Vector3(0.0001, 0.0002, 0.0003),
	"Normalized": Vector3(3, 4, 5).normalized()
}

func _init():
	test_name = "Vector3"

## Test basic serialization/deserialization without styles
func test_basic_serialization() -> void:
	run_basic_serialization_test(test_values, is_vector3_equal)

## Test different container forms (map vs sequence)
func test_container_forms() -> void:
	var vec = Vector3(10, 20, 30)
	run_container_forms_test(vec, is_vector3_equal, ["x", "y", "z"])

## Test different flow styles (block vs flow)
func test_flow_styles() -> void:
	var vec = Vector3(10, 20, 30)
	run_flow_styles_test(vec, is_vector3_equal)

## Test roundtrip conversion with style detection enabled
func test_roundtrip_with_styles() -> void:
	var vec = Vector3(10, 20, 30)

	run_style_roundtrip_test(
		vec,
		is_vector3_equal,
		func(parsed_vec): return parsed_vec * 2
	)

## Test error handling for invalid YAML
func test_parsing_errors() -> void:
	# Test Vector3-specific parsing errors
	var invalid_vector3_yaml = """
!Vector3
x: 1.0
y: 2.0
# Missing z field
"""
	assert_parse_error(invalid_vector3_yaml, "Missing required field detection")

	var wrong_type_yaml = """
!Vector3 "not a vector3"
"""
	assert_parse_error(wrong_type_yaml, "Wrong type/format detection")

	var invalid_sequence_length = """
!Vector3
- 1.0
- 2.0
"""
	assert_parse_error(invalid_sequence_length, "Invalid sequence length detection")

## Test special values (INF, NAN)
func test_special_values() -> void:
	var special_values = {
		"Infinity": Vector3(INF, INF, INF),
		"Negative Infinity": Vector3(-INF, -INF, -INF),
		"Mixed Infinity": Vector3(INF, -INF, INF),
		"NaN": Vector3(NAN, NAN, NAN)
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
			elif name == "Negative Infinity":
				assert_true(is_inf(parsed_vec.x) && parsed_vec.x < 0, "Negative infinity preserved for x")
				assert_true(is_inf(parsed_vec.y) && parsed_vec.y < 0, "Negative infinity preserved for y")
				assert_true(is_inf(parsed_vec.z) && parsed_vec.z < 0, "Negative infinity preserved for z")
			elif name == "Mixed Infinity":
				assert_true(is_inf(parsed_vec.x) && parsed_vec.x > 0, "Positive infinity preserved for x")
				assert_true(is_inf(parsed_vec.y) && parsed_vec.y < 0, "Negative infinity preserved for y")
				assert_true(is_inf(parsed_vec.z) && parsed_vec.z > 0, "Positive infinity preserved for z")
			elif name == "NaN":
				assert_true(is_nan(parsed_vec.x), "NaN preserved for x")
				assert_true(is_nan(parsed_vec.y), "NaN preserved for y")
				assert_true(is_nan(parsed_vec.z), "NaN preserved for z")

## Helper function to check if Vector3 instances are equal (with floating point precision)
func is_vector3_equal(a: Vector3, b: Vector3, epsilon: float = 0.00001) -> bool:
	return a.is_equal_approx(b)

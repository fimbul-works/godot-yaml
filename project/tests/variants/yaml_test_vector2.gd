extends YAMLTest
## Test suite for Vector2 YAML serialization and styling

# Test values covering different vector configurations
var test_values = {
	"Zero": Vector2.ZERO,
	"One": Vector2.ONE,
	"Unit_x": Vector2.RIGHT,
	"Unit_y": Vector2.UP,
	"Simple": Vector2(10, 20),
	"Negative": Vector2(-30, -40),
	"Mixed": Vector2(50, -60),
	"Decimal": Vector2(1.5, 2.75),
	"Large": Vector2(1000, 2000),
	"Very_small": Vector2(0.0001, 0.0002),
	"Normalized": Vector2(3, 4).normalized(),
}

func _init():
	test_name = "Vector2"

## Test basic serialization/deserialization without styles
func test_basic_serialization() -> void:
	run_basic_serialization_test(test_values, is_vector2_equal)

## Test different container forms (map vs sequence)
func test_container_forms() -> void:
	var vec = Vector2(10, 20)
	run_container_forms_test(vec, is_vector2_equal, ["x", "y"])

## Test different flow styles (block vs flow)
func test_flow_styles() -> void:
	var vec = Vector2(10, 20)
	run_flow_styles_test(vec, is_vector2_equal)

## Test roundtrip conversion with style detection enabled
func test_roundtrip_with_styles() -> void:
	var vec = Vector2(10, 20)

	run_style_roundtrip_test(
		vec,
		is_vector2_equal,
		func(parsed_vec): return parsed_vec * 2
	)

## Test error handling for invalid YAML
func test_parsing_errors() -> void:
	# Test Vector2-specific parsing errors
	var invalid_vector2_yaml = """
!Vector2
x: 1.0
# Missing y field
"""
	assert_parse_error(invalid_vector2_yaml, "Missing required field detection")

	var wrong_type_yaml = """
!Vector2 "not a vector2"
"""
	assert_parse_error(wrong_type_yaml, "Wrong type/format detection")

	var invalid_sequence_length = """
!Vector2
- 1.0
- 2.0
- 3.0
"""
	assert_parse_error(invalid_sequence_length, "Invalid sequence length detection")

## Test special values (INF, NAN)
func test_special_values() -> void:
	var special_values = {
		"inf": Vector2(INF, INF),
		"neg_inf": Vector2(-INF, -INF),
		"mixed_inf": Vector2(INF, -INF)
	}

	for name in special_values:
		var vec = special_values[name]
		var result = YAML.stringify(vec)

		assert_stringify_success(result, "special value: " + name)
		if result.has_error():
			continue

		print_rich("• %s: %s" % [name, result.get_data()])

		# Parse back and check (skip comparison for NaN)
		var parse_result = YAML.parse(result.get_data())
		assert_parse_success(parse_result, "parse special value: " + name)

		if !parse_result.has_error():
			var parsed_vec = parse_result.get_data()

			# Check INF values are preserved
			if name == "inf":
				assert_true(is_inf(parsed_vec.x) && parsed_vec.x > 0, "Positive infinity preserved")
				assert_true(is_inf(parsed_vec.y) && parsed_vec.y > 0, "Positive infinity preserved")
			elif name == "neg_inf":
				assert_true(is_inf(parsed_vec.x) && parsed_vec.x < 0, "Negative infinity preserved")
				assert_true(is_inf(parsed_vec.y) && parsed_vec.y < 0, "Negative infinity preserved")
			elif name == "mixed_inf":
				assert_true(is_inf(parsed_vec.x) && parsed_vec.x > 0, "Positive infinity preserved")
				assert_true(is_inf(parsed_vec.y) && parsed_vec.y < 0, "Negative infinity preserved")

## Helper function to check if Vector2 instances are equal (with floating point precision)
func is_vector2_equal(a: Vector2, b: Vector2, epsilon: float = 0.00001) -> bool:
	return a.is_equal_approx(b)

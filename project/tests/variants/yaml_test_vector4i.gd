extends YAMLTest
## Test suite for Vector4i YAML serialization and styling

# Test values with different vector configurations
var test_values = {
	"Zero": Vector4i.ZERO,
	"One": Vector4i.ONE,
	"Simple": Vector4i(10, 20, 30, 40),
	"Negative": Vector4i(-50, -60, -70, -80),
	"Mixed": Vector4i(90, -100, 110, -120),
	"Large": Vector4i(1000, 2000, 3000, 4000),
	"Max": Vector4i(2147483647, 2147483647, 2147483647, 2147483647),
	"Min": Vector4i(-2147483647, -2147483647, -2147483647, -2147483647)
}

func _init():
	test_name = "Vector4i"

## Test basic serialization/deserialization without styles
func test_basic_serialization() -> void:
	run_basic_serialization_test(test_values, is_vector4i_equal)

## Test different container forms (map vs sequence)
func test_container_forms() -> void:
	var vec = Vector4i(10, 20, 30, 40)
	run_container_forms_test(vec, is_vector4i_equal, ["x", "y", "z", "w"])

## Test different flow styles (block vs flow)
func test_flow_styles() -> void:
	var vec = Vector4i(10, 20, 30, 40)
	run_flow_styles_test(vec, is_vector4i_equal)

## Test roundtrip conversion with style detection enabled
func test_roundtrip_with_styles() -> void:
	var vec = Vector4i(10, 20, 30, 40)

	run_style_roundtrip_test(
		vec,
		is_vector4i_equal,
		func(parsed_vec): return parsed_vec * 2
	)

## Test error handling for invalid YAML
func test_parsing_errors() -> void:
	# Test Vector4i-specific parsing errors
	var invalid_vector4i_yaml = """
!Vector4i
x: 1
y: 2
z: 3
# Missing w field
"""
	assert_parse_error(invalid_vector4i_yaml, "Missing required field detection")

	var wrong_type_yaml = """
!Vector4i "not a vector4i"
"""
	assert_parse_error(wrong_type_yaml, "Wrong type/format detection")

	var invalid_sequence_length = """
!Vector4i
- 1
- 2
- 3
"""
	assert_parse_error(invalid_sequence_length, "Invalid sequence length detection")

	var float_for_int = """
!Vector4i
x: 1.5
y: 2.5
z: 3.5
w: 4.5
"""
	var float_result = YAML.parse(float_for_int)
	if !float_result.has_error():
		# Floats should be truncated to integers for Vector4i
		var vec = float_result.get_data()
		assert_equal(vec.x, 1, "Float value should be truncated to integer")
		assert_equal(vec.y, 2, "Float value should be truncated to integer")
		assert_equal(vec.z, 3, "Float value should be truncated to integer")
		assert_equal(vec.w, 4, "Float value should be truncated to integer")

## Test number format variations
func test_number_formats() -> void:
	# Test various integer formats
	var formats = {
		"Decimal": "!Vector4i [10, 20, 30, 40]",
		"Hex": "!Vector4i [0x0A, 0x14, 0x1E, 0x28]",
		"Octal": "!Vector4i [0o12, 0o24, 0o36, 0o50]",
		"Binary": "!Vector4i [0b1010, 0b10100, 0b11110, 0b101000]"
	}

	for format_name in formats:
		var yaml_str = formats[format_name]
		var parse_result = YAML.parse(yaml_str)

		assert_parse_success(parse_result, "Parse format: " + format_name)
		if parse_result.has_error():
			continue

		var vec = parse_result.get_data()
		print_rich("• %s format: %s → Vector4i(%d, %d, %d, %d)" % [
			format_name, yaml_str, vec.x, vec.y, vec.z, vec.w
		])

		# All formats should parse to the same values
		assert_equal(vec.x, 10, format_name + " x value correct")
		assert_equal(vec.y, 20, format_name + " y value correct")
		assert_equal(vec.z, 30, format_name + " z value correct")
		assert_equal(vec.w, 40, format_name + " w value correct")

## Test specific Vector4i operations
func test_vector4i_operations() -> void:
	# Verify operations on integers work as expected
	var vec = Vector4i(1, 2, 3, 4)

	# Serialize and parse
	var result = YAML.stringify(vec)
	assert_stringify_success(result, "Vector4i operation test")

	if !result.has_error():
		var parse_result = YAML.parse(result.get_data())
		assert_parse_success(parse_result, "Parse vector4i")

		if !parse_result.has_error():
			var parsed_vec = parse_result.get_data()

			# Test operations
			# 1. Addition
			var sum_original = vec + Vector4i(5, 6, 7, 8)
			var sum_parsed = parsed_vec + Vector4i(5, 6, 7, 8)
			assert_equal(sum_original, sum_parsed, "Addition operation preserved")

			# 2. Multiplication
			var mult_original = vec * 2
			var mult_parsed = parsed_vec * 2
			assert_equal(mult_original, mult_parsed, "Multiplication operation preserved")

			# 3. Length (should be calculated the same)
			var length_original = vec.length()
			var length_parsed = parsed_vec.length()
			assert_equal(length_original, length_parsed, "Length calculation preserved")

## Helper function to check if Vector4i instances are equal
func is_vector4i_equal(a: Vector4i, b: Vector4i) -> bool:
	return a == b

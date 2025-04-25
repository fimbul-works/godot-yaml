extends YAMLTest
## Test suite for Vector2i YAML serialization and styling

# Test values with different vector configurations
var test_values = {
	"Zero": Vector2i.ZERO,
	"One": Vector2i.ONE,
	"Up": Vector2i.UP,
	"Down": Vector2i.DOWN,
	"Left": Vector2i.LEFT,
	"Right": Vector2i.RIGHT,
	"Simple": Vector2i(10, 20),
	"Negative": Vector2i(-30, -40),
	"Mixed": Vector2i(50, -60),
	"Large": Vector2i(1000, 2000),
	"Max": Vector2i(2147483647, 2147483647),
	"Min": Vector2i(-2147483647, -2147483647)
}

func _init():
	test_name = "Vector2i"

## Test basic serialization/deserialization without styles
func test_basic_serialization() -> void:
	run_basic_serialization_test(test_values, is_vector2i_equal)

## Test different container forms (map vs sequence)
func test_container_forms() -> void:
	var vec = Vector2i(10, 20)
	run_container_forms_test(vec, is_vector2i_equal, ["x", "y"])

## Test different flow styles (block vs flow)
func test_flow_styles() -> void:
	var vec = Vector2i(10, 20)
	run_flow_styles_test(vec, is_vector2i_equal)

## Test roundtrip conversion with style detection enabled
func test_roundtrip_with_styles() -> void:
	var vec = Vector2i(10, 20)

	run_style_roundtrip_test(
		vec,
		is_vector2i_equal,
		func(parsed_vec): return parsed_vec * 2
	)

## Test error handling for invalid YAML
func test_parsing_errors() -> void:
	# Test Vector2i-specific parsing errors
	var invalid_vector2i_yaml = """
!Vector2i
x: 1
# Missing y field
"""
	assert_parse_error(invalid_vector2i_yaml, "Missing required field detection")

	var wrong_type_yaml = """
!Vector2i "not a vector2i"
"""
	assert_parse_error(wrong_type_yaml, "Wrong type/format detection")

	var invalid_sequence_length = """
!Vector2i
- 1
- 2
- 3
"""
	assert_parse_error(invalid_sequence_length, "Invalid sequence length detection")

	var float_for_int = """
!Vector2i
x: 1.5
y: 2.5
"""
	var float_result = YAML.parse(float_for_int)
	if !float_result.has_error():
		# Floats should be truncated to integers for Vector2i
		var vec = float_result.get_data()
		assert_equal(vec.x, 1, "Float value should be truncated to integer")
		assert_equal(vec.y, 2, "Float value should be truncated to integer")

## Test number format variations
func test_number_formats() -> void:
	# Test various integer formats
	var formats = {
		"Decimal": "!Vector2i [10, 20]",
		"Hex": "!Vector2i [0x0A, 0x14]",
		"Octal": "!Vector2i [0o12, 0o24]",
		"Binary": "!Vector2i [0b1010, 0b10100]"
	}

	for format_name in formats:
		var yaml_str = formats[format_name]
		var parse_result = YAML.parse(yaml_str)

		assert_parse_success(parse_result, "Parse format: " + format_name)
		if parse_result.has_error():
			continue

		var vec = parse_result.get_data()
		print_rich("• %s format: %s → Vector2i(%d, %d)" % [
			format_name, yaml_str, vec.x, vec.y
		])

		# All formats should parse to the same values
		assert_equal(vec.x, 10, format_name + " x value correct")
		assert_equal(vec.y, 20, format_name + " y value correct")

## Helper function to check if Vector2i instances are equal
func is_vector2i_equal(a: Vector2i, b: Vector2i) -> bool:
	return a == b

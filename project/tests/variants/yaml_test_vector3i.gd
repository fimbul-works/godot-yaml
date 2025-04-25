extends YAMLTest
## Test suite for Vector3i YAML serialization and styling

# Test values with different vector configurations
var test_values = {
	"Zero": Vector3i.ZERO,
	"One": Vector3i.ONE,
	"Up": Vector3i.UP,
	"Down": Vector3i.DOWN,
	"Left": Vector3i.LEFT,
	"Right": Vector3i.RIGHT,
	"Forward": Vector3i.FORWARD,
	"Back": Vector3i.BACK,
	"Simple": Vector3i(10, 20, 30),
	"Negative": Vector3i(-40, -50, -60),
	"Mixed": Vector3i(70, -80, 90),
	"Large": Vector3i(1000, 2000, 3000),
	"Max": Vector3i(2147483647, 2147483647, 2147483647),
	"Min": Vector3i(-2147483647, -2147483647, -2147483647)
}

func _init():
	test_name = "Vector3i"

## Test basic serialization/deserialization without styles
func test_basic_serialization() -> void:
	run_basic_serialization_test(test_values, is_vector3i_equal)

## Test different container forms (map vs sequence)
func test_container_forms() -> void:
	var vec = Vector3i(10, 20, 30)
	run_container_forms_test(vec, is_vector3i_equal, ["x", "y", "z"])

## Test different flow styles (block vs flow)
func test_flow_styles() -> void:
	var vec = Vector3i(10, 20, 30)
	run_flow_styles_test(vec, is_vector3i_equal)

## Test roundtrip conversion with style detection enabled
func test_roundtrip_with_styles() -> void:
	var vec = Vector3i(10, 20, 30)

	run_style_roundtrip_test(
		vec,
		is_vector3i_equal,
		func(parsed_vec): return parsed_vec * 2
	)

## Test error handling for invalid YAML
func test_parsing_errors() -> void:
	# Test Vector3i-specific parsing errors
	var invalid_vector3i_yaml = """
!Vector3i
x: 1
y: 2
# Missing z field
"""
	assert_parse_error(invalid_vector3i_yaml, "Missing required field detection")

	var wrong_type_yaml = """
!Vector3i "not a vector3i"
"""
	assert_parse_error(wrong_type_yaml, "Wrong type/format detection")

	var invalid_sequence_length = """
!Vector3i
- 1
- 2
"""
	assert_parse_error(invalid_sequence_length, "Invalid sequence length detection")

	var float_for_int = """
!Vector3i
x: 1.5
y: 2.5
z: 3.5
"""
	var float_result = YAML.parse(float_for_int)
	if !float_result.has_error():
		# Floats should be truncated to integers for Vector3i
		var vec = float_result.get_data()
		assert_equal(vec.x, 1, "Float value should be truncated to integer")
		assert_equal(vec.y, 2, "Float value should be truncated to integer")
		assert_equal(vec.z, 3, "Float value should be truncated to integer")

## Test number format variations
func test_number_formats() -> void:
	# Test various integer formats
	var formats = {
		"Decimal": "!Vector3i [10, 20, 30]",
		"Hex": "!Vector3i [0x0A, 0x14, 0x1E]",
		"Octal": "!Vector3i [0o12, 0o24, 0o36]",
		"Binary": "!Vector3i [0b1010, 0b10100, 0b11110]"
	}

	for format_name in formats:
		var yaml_str = formats[format_name]
		var parse_result = YAML.parse(yaml_str)

		assert_parse_success(parse_result, "Parse format: " + format_name)
		if parse_result.has_error():
			continue

		var vec = parse_result.get_data()
		print_rich("• %s format: %s → Vector3i(%d, %d, %d)" % [
			format_name, yaml_str, vec.x, vec.y, vec.z
		])

		# All formats should parse to the same values
		assert_equal(vec.x, 10, format_name + " x value correct")
		assert_equal(vec.y, 20, format_name + " y value correct")
		assert_equal(vec.z, 30, format_name + " z value correct")

## Helper function to check if Vector3i instances are equal
func is_vector3i_equal(a: Vector3i, b: Vector3i) -> bool:
	return a == b

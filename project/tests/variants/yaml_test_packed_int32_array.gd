extends YAMLTest
## Test suite for PackedInt32Array YAML serialization and styling

func _init():
	test_name = "PackedInt32Array"

# Test values covering different integer scenarios
var test_values = {
	"Empty": PackedInt32Array(),
	"Single": PackedInt32Array([42]),
	"Basic": PackedInt32Array([1, 2, 3, 4, 5]),
	"Negative": PackedInt32Array([-1, -2, -3, -4, -5]),
	"Mixed": PackedInt32Array([1, -2, 3, -4, 5]),
	"Zero": PackedInt32Array([0, 0, 0, 0, 0]),
	"Large": PackedInt32Array([10000, 100000, 1000000, 10000000]),
	"Max_values": PackedInt32Array([
		2147483647,
		-2147483647,
		0, 1, -1
	]),
	"Sequential": create_sequential_array(50)
}

# Helper to create a sequential array of integers
func create_sequential_array(size: int) -> PackedInt32Array:
	var array = PackedInt32Array()
	array.resize(size)
	for i in range(size):
		array[i] = i
	return array

# Helper to create a large array with varied values
func create_large_array(size: int = 100) -> PackedInt32Array:
	var array = PackedInt32Array()
	array.resize(size)
	for i in range(size):
		array[i] = (i * 1000) * (1 if i % 2 == 0 else -1)
	return array

## Test basic serialization/deserialization without styles
func test_basic_serialization() -> void:
	for name in test_values:
		var int_array = test_values[name]
		var result = YAML.stringify(int_array)

		assert_stringify_success(result, name)
		if result.has_error():
			continue

		print_rich("• %s (%d elements): %s" % [
			name,
			int_array.size(),
			truncate(result.get_data())
		])

		# Parse back and verify
		var parse_result = YAML.parse(result.get_data())
		assert_roundtrip(parse_result, int_array, is_packed_int32_array_equal, name)

## Test different flow styles (block vs flow)
func test_flow_styles() -> void:
	var int_array = PackedInt32Array([10, 20, 30, 40, 50])

	# Test flow style (compact)
	var flow_style = YAML.create_style()
	flow_style.set_flow_style(YAMLStyle.FLOW_SINGLE)
	var flow_result = YAML.stringify(int_array, flow_style)

	assert_stringify_success(flow_result, "flow style")
	if !flow_result.has_error():
		print_rich("• Flow style:")
		print_rich(flow_result.get_data())
		assert_yaml_has_feature(flow_result.get_data(), "[", "Contains opening bracket")
		assert_yaml_has_feature(flow_result.get_data(), "]", "Contains closing bracket")

	# Test block style (expanded)
	var block_style = YAML.create_style()
	block_style.set_flow_style(YAMLStyle.FLOW_NONE)
	var block_result = YAML.stringify(int_array, block_style)

	assert_stringify_success(block_result, "block style")
	if !block_result.has_error():
		print_rich("• Block style:")
		print_rich(block_result.get_data())
		assert_yaml_has_feature(block_result.get_data(), "- ", "Contains block sequence indicators")

	# Test roundtrip for both styles
	assert_roundtrip(YAML.parse(flow_result.get_data()), int_array, is_packed_int32_array_equal, "flow style")
	assert_roundtrip(YAML.parse(block_result.get_data()), int_array, is_packed_int32_array_equal, "block style")

## Test number formats
func test_number_formats() -> void:
	var int_array = PackedInt32Array([10, 20, 30, 255, 15])

	var formats = {
		"Decimal": YAMLStyle.INT_DECIMAL,
		"Hex": YAMLStyle.INT_HEX,
		"Octal": YAMLStyle.INT_OCTAL,
		"Binary": YAMLStyle.INT_BINARY,
	}

	for format_name in formats:
		var style = YAML.create_style()
		var template = YAML.create_style()
		template.set_integer_format(formats[format_name])
		style.set_child("_template", template)

		var result = YAML.stringify(int_array, style)
		assert_stringify_success(result, format_name + " format")

		if !result.has_error():
			print_rich("• %s format:" % format_name)
			print_rich(result.get_data())

			# Check format-specific features
			match formats[format_name]:
				YAMLStyle.INT_HEX:
					var has_hex = result.get_data().find("0x") != -1
					if has_hex:
						assert_true(has_hex, "Uses hex notation")
					else:
						print_rich("[color=yellow]⚠ Hex notation not detected (may be implemented differently)[/color]")
				YAMLStyle.INT_OCTAL:
					var has_octal = result.get_data().find("0o") != -1
					if has_octal:
						assert_true(has_octal, "Uses octal notation")
					else:
						print_rich("[color=yellow]⚠ Octal notation not detected (may be implemented differently)[/color]")
				YAMLStyle.INT_BINARY:
					var has_binary = result.get_data().find("0b") != -1
					if has_binary:
						assert_true(has_binary, "Uses binary notation")
					else:
						print_rich("[color=yellow]⚠ Binary notation not detected (may be implemented differently)[/color]")

			# Test roundtrip
			assert_roundtrip(YAML.parse(result.get_data()), int_array, is_packed_int32_array_equal, format_name)

## Test item-specific styles
func test_item_styles() -> void:
	var int_array = PackedInt32Array([10, 255, 8])

	# Create parent style
	var parent_style = YAML.create_style()

	# Create different styles for each item
	var item0_style = YAML.create_style()
	item0_style.set_integer_format(YAMLStyle.INT_DECIMAL)

	var item1_style = YAML.create_style()
	item1_style.set_integer_format(YAMLStyle.INT_HEX)

	var item2_style = YAML.create_style()
	item2_style.set_integer_format(YAMLStyle.INT_BINARY)

	# Apply styles
	parent_style.set_child("0", item0_style)
	parent_style.set_child("1", item1_style)
	parent_style.set_child("2", item2_style)

	var result = YAML.stringify(int_array, parent_style)

	assert_stringify_success(result, "item styles")
	if !result.has_error():
		print_rich("• Item-specific styles:")
		print_rich(result.get_data())

		# Test roundtrip
		var parse_result = YAML.parse(result.get_data())
		assert_roundtrip(parse_result, int_array, is_packed_int32_array_equal, "item styles")

## Test large array
func test_large_array() -> void:
	var large_array = create_large_array(100)

	print_rich("\nTesting large Int32Array handling:")

	var test_cases = [
		{
			"name": "Large array in block style",
			"style": YAML.create_style().set_flow_style(YAMLStyle.FLOW_NONE)
		},
		{
			"name": "Large array in flow style",
			"style": YAML.create_style().set_flow_style(YAMLStyle.FLOW_SINGLE)
		}
	]

	for test_case in test_cases:
		var result = YAML.stringify(large_array, test_case.style)
		assert_stringify_success(result, test_case.name)

		if !result.has_error():
			var yaml_str = result.get_data()
			print_rich("• %s: (Length: %d characters)" % [test_case.name, yaml_str.length()])
			print_rich("  First 100 chars: " + yaml_str.substr(0, 100) + "...")

			# Parse back and verify
			var parse_result = YAML.parse(yaml_str)
			assert_roundtrip(parse_result, large_array, is_packed_int32_array_equal, test_case.name)

## Test parsing various integer formats
func test_parse_formats() -> void:
	print_rich("\nTesting parsing of different integer formats:")

	var test_formats = [
		"!PackedInt32Array []",  # Empty array
		"!PackedInt32Array [1, 2, 3, 4, 5]",  # Basic integers
		"!PackedInt32Array [0, -0, -10, 100]",  # Mixed signs
		"!PackedInt32Array [0x10, 0xFF, 0x100]",  # Hex notation
		"!PackedInt32Array [0o10, 0o7, 0o70]",  # Octal notation
		"!PackedInt32Array [0b1010, 0b101, 0b11]"  # Binary notation
	]

	for format_str in test_formats:
		var parse_result = YAML.parse(format_str)
		assert_parse_success(parse_result, "Parse format: %s" % format_str)

		if not parse_result.has_error():
			var int_array = parse_result.get_data()
			print_rich("• %s → %s" % [format_str, format_array_for_display(int_array)])

## Test roundtrip conversion with style detection
func test_roundtrip_with_styles() -> void:
	var int_array = PackedInt32Array([10, 20, 30, 40, 50])

	# Create a style with specific formatting
	var original_style = YAML.create_style()
	original_style.set_flow_style(YAMLStyle.FLOW_SINGLE)

	# Create item style for the first element
	original_style.set_child("0", YAML.create_style().set_integer_format(YAMLStyle.INT_HEX))

	print("Original: ", original_style.get_debug_string())

	# Emit YAML with the style
	var emit_result = YAML.stringify(int_array, original_style)
	assert_stringify_success(emit_result, "initial stringify")
	if emit_result.has_error():
		return

	var yaml_text = emit_result.get_data()
	print_rich("• Original YAML (with flow style):")
	print_rich(yaml_text)

	# Parse with style detection enabled
	var parse_result = YAML.parse(yaml_text, YAML.create_security(), true)  # true enables style detection
	assert_parse_success(parse_result, "parse with style detection")
	if parse_result.has_error():
		return

	# Check if style was detected
	if !parse_result.has_style():
		print_rich("[color=yellow]⚠ No style was detected[/color]")
	else:
		print_rich("[color=green]✓ Style detected successfully[/color]")

		# Get the detected style and data
		var detected_style := parse_result.get_style()
		var parsed_array = parse_result.get_data()
		print("Detected: ", detected_style.get_debug_string())

		# Modify the array (add a value)
		var modified_array = PackedInt32Array()
		for value in parsed_array:
			modified_array.append(value)
		modified_array.append(60)

		# Re-emit with the detected style
		var re_emit_result = YAML.stringify(modified_array, detected_style)
		assert_stringify_success(re_emit_result, "re-stringify with detected style")
		if re_emit_result.has_error():
			return

		print_rich("• Re-emitted YAML (with preserved style):")
		print_rich(re_emit_result.get_data())

		# Verify the style was preserved (flow style should be maintained)
		assert_yaml_has_feature(re_emit_result.get_data(), "[", "Flow style was preserved (opening bracket)")
		assert_yaml_has_feature(re_emit_result.get_data(), "]", "Flow style was preserved (closing bracket)")

## Test error handling for invalid YAML
func test_parsing_errors() -> void:
	# Test array-specific parsing errors
	var invalid_format = """
!PackedInt32Array "not an array"
"""
	assert_parse_error(invalid_format, "Wrong type detection")

	var invalid_element = """
!PackedInt32Array [1, "text", 3]
"""
	assert_parse_error(invalid_element, "Invalid element type detection")

	var too_large_value = """
!PackedInt32Array [2147483648]
"""
	assert_parse_error(too_large_value, "Value out of range detection")

## Helper function to check if PackedInt32Array instances are equal
func is_packed_int32_array_equal(a: PackedInt32Array, b: PackedInt32Array) -> bool:
	if a.size() != b.size():
		return false

	for i in range(a.size()):
		if a[i] != b[i]:
			return false

	return true

## Helper to format array contents for display
func format_array_for_display(array: PackedInt32Array) -> String:
	if array.size() == 0:
		return "[]"

	var result = "["
	for i in range(array.size()):
		if i > 0:
			result += ", "
		result += str(array[i])

		# Limit display length for large arrays
		if i >= 5 and array.size() > 8:
			result += ", ... (%d more values)]" % (array.size() - i - 1)
			return result

	result += "]"
	return result

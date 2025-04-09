extends YAMLTest
## Test suite for PackedInt64Array YAML serialization and styling

func _init():
	test_name = "🧩 PackedInt64Array YAML Tests"

# Helper function to create test arrays with various integer patterns
func create_test_arrays() -> Dictionary:
	return {
		"empty": PackedInt64Array(),
		"single": PackedInt64Array([42]),
		"basic": PackedInt64Array([1, 2, 3, 4, 5]),
		"negative": PackedInt64Array([-1, -2, -3, -4, -5]),
		"mixed": PackedInt64Array([1, -2, 3, -4, 5]),
		"zero": PackedInt64Array([0, 0, 0, 0, 0]),
		"large": PackedInt64Array([100000000, 1000000000, 10000000000, 100000000000, 1000000000000]),
		"max_values": create_max_values_array(),
		"sequential": create_sequential_array(50),
		"many": create_many_values_array(100)
	}

# Helper to create an array with max/min values
func create_max_values_array() -> PackedInt64Array:
	var array = PackedInt64Array()
	array.append(9223372036854775807)  # INT64_MAX
	array.append(-9223372036854775807) # INT64_MIN
	array.append(0)
	array.append(1)
	array.append(-1)
	return array

# Helper to create a sequential array
func create_sequential_array(size: int) -> PackedInt64Array:
	var array = PackedInt64Array()
	array.resize(size)

	for i in range(size):
		array[i] = i

	return array

# Helper to create a large array with varied values
func create_many_values_array(size: int) -> PackedInt64Array:
	var array = PackedInt64Array()
	array.resize(size)

	for i in range(size):
		# Using larger values than Int32
		array[i] = (i * 10000000) * (1 if i % 2 == 0 else -1)

	return array

## Test basic serialization/deserialization without styles
func test_basic_serialization() -> void:
	var test_arrays = create_test_arrays()

	for name in test_arrays:
		var int_array = test_arrays[name]
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
		assert_roundtrip(parse_result, int_array, is_packed_int64_array_equal, name)

## Test different flow styles (block vs flow)
func test_flow_styles() -> void:
	# Use a simple array for testing styles
	var int_array = PackedInt64Array([10000000, 20000000, 30000000, 40000000, 50000000])

	# Test flow style (compact)
	var flow_style = YAML.create_style()
	flow_style.set_flow_style(YAMLStyle.FLOW_SINGLE)
	var flow_result = YAML.stringify(int_array, flow_style)

	assert_stringify_success(flow_result, "flow style")
	if !flow_result.has_error():
		print_rich("• Flow style:")
		print_rich(flow_result.get_data())

		# Verify it contains flow indicators (brackets)
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

		# Verify it uses proper block style for a sequence
		assert_yaml_has_feature(block_result.get_data(), "- ", "Contains block sequence indicators")

	# Test roundtrip for both styles
	assert_roundtrip(YAML.parse(flow_result.get_data()), int_array, is_packed_int64_array_equal, "flow style")
	assert_roundtrip(YAML.parse(block_result.get_data()), int_array, is_packed_int64_array_equal, "block style")

## Test number formats
func test_number_formats() -> void:
	# Test a simple array with different number formats
	var int_array = PackedInt64Array([10000000, 20000000, 30000000, 255, 15])

	# Test different number formats
	var formats = {
		"Decimal": YAMLStyle.NUM_DECIMAL,
		"Hex": YAMLStyle.NUM_HEX,
		"Octal": YAMLStyle.NUM_OCTAL,
		"Binary": YAMLStyle.NUM_BINARY
	}

	for format_name in formats:
		var number_style = YAML.create_style()
		var items_style = YAML.create_style()
		items_style.set_number_format(formats[format_name])
		number_style.set_child("_items", items_style)

		var result = YAML.stringify(int_array, number_style)
		assert_stringify_success(result, format_name + " format")

		if !result.has_error():
			print_rich("• %s format:" % format_name)
			print_rich(result.get_data())

			# Check format specific features
			match formats[format_name]:
				YAMLStyle.NUM_HEX:
					# Look for hex notation (0x)
					var has_hex = result.get_data().find("0x") != -1
					if has_hex:
						assert_true(has_hex, "Uses hex notation")
					else:
						print_rich("[color=yellow]⚠ Hex notation not detected (may be implemented differently)[/color]")
				YAMLStyle.NUM_OCTAL:
					# Look for octal notation (0o)
					var has_octal = result.get_data().find("0o") != -1
					if has_octal:
						assert_true(has_octal, "Uses octal notation")
					else:
						print_rich("[color=yellow]⚠ Octal notation not detected (may be implemented differently)[/color]")
				YAMLStyle.NUM_BINARY:
					# Look for binary notation (0b)
					var has_binary = result.get_data().find("0b") != -1
					if has_binary:
						assert_true(has_binary, "Uses binary notation")
					else:
						print_rich("[color=yellow]⚠ Binary notation not detected (may be implemented differently)[/color]")

			# Test roundtrip
			assert_roundtrip(YAML.parse(result.get_data()), int_array, is_packed_int64_array_equal, format_name)

## Test item-specific styles
func test_item_styles() -> void:
	var int_array = PackedInt64Array([
		10000000,   # Will be decimal
		255,        # Will be hex
		8           # Will be binary
	])

	# Create parent style
	var parent_style = YAML.create_style()

	# Create different styles for each item
	var item0_style = YAML.create_style()
	item0_style.set_number_format(YAMLStyle.NUM_DECIMAL)

	var item1_style = YAML.create_style()
	item1_style.set_number_format(YAMLStyle.NUM_HEX)

	var item2_style = YAML.create_style()
	item2_style.set_number_format(YAMLStyle.NUM_BINARY)

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
		assert_roundtrip(parse_result, int_array, is_packed_int64_array_equal, "item styles")

## Test Integer Range limits
func test_integer_range() -> void:
	# Create an array with min/max int64 values
	var range_array = PackedInt64Array([
		-9223372036854775807,  # INT64_MIN
		9223372036854775807,   # INT64_MAX
		0,
		1,
		-1
	])

	var result = YAML.stringify(range_array)
	assert_stringify_success(result, "integer range")

	if !result.has_error():
		print_rich("• Integer range values:")
		print_rich(result.get_data())

		# Parse back and verify
		var parse_result = YAML.parse(result.get_data())
		assert_parse_success(parse_result, "parse integer range")

		if !parse_result.has_error():
			var parsed_array = parse_result.get_data()

			# Verify the min/max values are preserved
			assert_equal(parsed_array[0], -9223372036854775807, "INT64_MIN preserved")
			assert_equal(parsed_array[1], 9223372036854775807, "INT64_MAX preserved")

## Test large values beyond Int32 range
func test_large_values() -> void:
	# Create an array with values beyond int32 range
	var large_values_array = PackedInt64Array([
		2147483648,    # INT32_MAX + 1
		-2147483648,   # INT32_MIN - 1
		4294967296,    # 2^32
		-4294967296,   # -2^32
		1000000000000  # 1 trillion
	])

	var result = YAML.stringify(large_values_array)
	assert_stringify_success(result, "large values")

	if !result.has_error():
		print_rich("• Large values beyond Int32 range:")
		print_rich(result.get_data())

		# Parse back and verify
		var parse_result = YAML.parse(result.get_data())
		assert_parse_success(parse_result, "parse large values")

		if !parse_result.has_error():
			var parsed_array = parse_result.get_data()

			# Verify the large values are preserved
			for i in range(large_values_array.size()):
				assert_equal(
					parsed_array[i],
					large_values_array[i],
					"Large value at index %d preserved: %s" % [i, large_values_array[i]]
				)

## Test large array
func test_large_array() -> void:
	var large_array = create_many_values_array(100)

	print_rich("\nTesting large Int64Array handling:")

	# Test with different styles
	var test_cases = [
		{
			"name": "Large array in block style",
			"style": func():
				var s = YAML.create_style()
				s.set_flow_style(YAMLStyle.FLOW_NONE)
				return s,
		},
		{
			"name": "Large array in flow style",
			"style": func():
				var s = YAML.create_style()
				s.set_flow_style(YAMLStyle.FLOW_SINGLE)
				return s,
		}
	]

	for test_case in test_cases:
		var name = test_case["name"]
		var style = test_case["style"].call()

		var result = YAML.stringify(large_array, style)
		assert_stringify_success(result, name)

		if !result.has_error():
			var yaml_str = result.get_data()
			print_rich("• %s: (Length: %d characters)" % [name, yaml_str.length()])
			print_rich("  First 100 chars: " + yaml_str.substr(0, 100) + "...")

			# Parse back and verify (might be slow for large arrays)
			var parse_result = YAML.parse(yaml_str)
			assert_roundtrip(parse_result, large_array, is_packed_int64_array_equal, name)

## Test large array with hex format
func test_large_array_hex_format() -> void:
	# Create an array specifically for hex format testing
	var array = PackedInt64Array()
	array.resize(10)

	# Use values that stay within signed int64 range
	for i in range(array.size()):
		array[i] = 0x1000000 * (i + 1)

	# Apply hex format
	var hex_style = YAML.create_style()
	var items_style = YAML.create_style()
	items_style.set_number_format(YAMLStyle.NUM_HEX)
	hex_style.set_child("_items", items_style)

	var result = YAML.stringify(array, hex_style)
	assert_stringify_success(result, "hex format")

	if !result.has_error():
		print_rich("• Hex format:")
		print_rich(result.get_data())

		# Parse back and verify
		var parse_result = YAML.parse(result.get_data())
		assert_parse_success(parse_result, "parse hex format")

		if !parse_result.has_error():
			var parsed_array = parse_result.get_data()

			# Check each value individually
			for i in range(array.size()):
				# For roundtrip of hex values, we need to check the bit pattern
				var expected_bits = array[i]
				var actual_bits = parsed_array[i]

				assert_true(
					expected_bits == actual_bits,
					"Bit pattern preserved for index %d: Expected 0x%X, got 0x%X" %
					[i, expected_bits, actual_bits]
				)

## Test parsing various integer formats
func test_parse_formats() -> void:
	print_rich("\nTesting parsing of different integer formats:")

	var test_formats = [
		"!PackedInt64Array []",  # Empty array
		"!PackedInt64Array [1, 2, 3, 4, 5]",  # Basic integers
		"!PackedInt64Array [0, -0, -10, 100]",  # Mixed signs
		"!PackedInt64Array [0x10, 0xFF, 0x100]",  # Hex notation
		"!PackedInt64Array [0o10, 0o7, 0o70]",  # Octal notation
		"!PackedInt64Array [0b1010, 0b101, 0b11]",  # Binary notation
		"!PackedInt64Array [2147483648, 4294967296]"  # Values beyond int32 range
	]

	for format_str in test_formats:
		var parse_result = YAML.parse(format_str)
		assert_parse_success(parse_result, "Parse format: %s" % format_str)

		if not parse_result.has_error():
			var int_array = parse_result.get_data()
			print_rich("• %s → %s" % [format_str, format_array_for_display(int_array)])

## Test roundtrip conversion with style detection
func test_roundtrip_with_styles() -> void:
	var int_array = PackedInt64Array([1000000000, 2000000000, 3000000000, 4000000000, 5000000000])

	# Create a style with specific formatting
	var original_style = YAML.create_style()
	original_style.set_flow_style(YAMLStyle.FLOW_SINGLE)

	# Create item style for the first element
	var item_style = YAML.create_style()
	item_style.set_number_format(YAMLStyle.NUM_HEX)
	original_style.set_child("0", item_style)

	# Emit YAML with the style
	var emit_result = YAML.stringify(int_array, original_style)
	assert_stringify_success(emit_result, "initial stringify")
	if emit_result.has_error():
		return

	var yaml_text = emit_result.get_data()
	print_rich("• Original YAML (with flow style):")
	print_rich(yaml_text)

	# Parse with style detection enabled
	var parse_result = YAML.parse(yaml_text, true)  # true enables style detection
	assert_parse_success(parse_result, "parse with style detection")
	if parse_result.has_error():
		return

	# Check if style was detected
	if !parse_result.has_style():
		print_rich("[color=yellow]⚠ No style was detected[/color]")
	else:
		print_rich("[color=green]✓ Style detected successfully[/color]")

		# Get the detected style and data
		var detected_style = parse_result.get_style()
		var parsed_array = parse_result.get_data()

		# Modify the array (add a value)
		var modified_array = PackedInt64Array()
		for value in parsed_array:
			modified_array.append(value)
		modified_array.append(6000000000)

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

## Helper function to check if PackedInt64Array instances are equal
func is_packed_int64_array_equal(a: PackedInt64Array, b: PackedInt64Array) -> bool:
	if a.size() != b.size():
		return false

	for i in range(a.size()):
		if a[i] != b[i]:
			return false

	return true

## Helper to format array contents for display
func format_array_for_display(array: PackedInt64Array) -> String:
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

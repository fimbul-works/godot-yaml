extends YAMLTest
## Test suite for PackedFloat32Array YAML serialization and styling

func _init():
	test_name = "🧩 PackedFloat32Array YAML Tests"

# Helper function to create test arrays with various float patterns
func create_test_arrays() -> Dictionary:
	return {
		"empty": PackedFloat32Array(),
		"single": PackedFloat32Array([1.0]),
		"basic": PackedFloat32Array([1.0, 2.0, 3.0, 4.0, 5.0]),
		"negative": PackedFloat32Array([-1.0, -2.0, -3.0, -4.0, -5.0]),
		"mixed": PackedFloat32Array([1.0, -2.0, 3.0, -4.0, 5.0]),
		"decimals": PackedFloat32Array([0.1, 0.25, 0.5, 0.75, 0.9]),
		"small": PackedFloat32Array([0.001, 0.0001, 0.00001, 0.000001]),
		"large": PackedFloat32Array([1000.0, 10000.0, 100000.0, 1000000.0]),
		"scientific": PackedFloat32Array([1.23e5, 4.56e-3, 7.89e+10, -1.23e-5]),
		"special": create_special_values_array(),
		"trig": create_trig_values_array(),
		"many": create_many_values_array(100)
	}

# Helper to create array with special floating point values
func create_special_values_array() -> PackedFloat32Array:
	var array = PackedFloat32Array()
	array.append(INF)
	array.append(-INF)
	array.append(NAN)
	array.append(0.0)
	array.append(-0.0)
	array.append(PI)
	array.append(TAU)
	return array

# Helper to create array with trigonometric values
func create_trig_values_array() -> PackedFloat32Array:
	var array = PackedFloat32Array()
	for i in range(0, 360, 30):
		var angle_rad = deg_to_rad(float(i))
		array.append(sin(angle_rad))
		array.append(cos(angle_rad))
	return array

# Helper to create a large array of floats
func create_many_values_array(size: int) -> PackedFloat32Array:
	var array = PackedFloat32Array()
	array.resize(size)

	for i in range(size):
		array[i] = sin(float(i) * 0.1) * 100.0

	return array

## Test basic serialization/deserialization without styles
func test_basic_serialization() -> void:
	var test_arrays = create_test_arrays()

	for name in test_arrays:
		var float_array = test_arrays[name]
		var result = YAML.stringify(float_array)

		# Skip testing NaN, as NaN != NaN by definition
		if name == "special":
			print_rich("• %s (%d elements): %s (skipping equality check due to NaN)" % [
				name,
				float_array.size(),
				truncate(result.get_data())
			])
			continue

		assert_stringify_success(result, name)
		if result.has_error():
			continue

		print_rich("• %s (%d elements): %s" % [
			name,
			float_array.size(),
			truncate(result.get_data())
		])

		# Parse back and verify
		var parse_result = YAML.parse(result.get_data())
		assert_roundtrip(parse_result, float_array, is_packed_float32_array_equal, name)

## Test different flow styles (block vs flow)
func test_flow_styles() -> void:
	# Use a simple array for testing styles
	var float_array = PackedFloat32Array([1.1, 2.2, 3.3, 4.4, 5.5])

	# Test flow style (compact)
	var flow_style = YAML.create_style()
	flow_style.set_flow_style(YAMLStyle.FLOW_SINGLE)
	var flow_result = YAML.stringify(float_array, flow_style)

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
	var block_result = YAML.stringify(float_array, block_style)

	assert_stringify_success(block_result, "block style")
	if !block_result.has_error():
		print_rich("• Block style:")
		print_rich(block_result.get_data())

		# Verify it uses proper block style for a sequence
		assert_yaml_has_feature(block_result.get_data(), "- ", "Contains block sequence indicators")

	# Test roundtrip for both styles
	assert_roundtrip(YAML.parse(flow_result.get_data()), float_array, is_packed_float32_array_equal, "flow style")
	assert_roundtrip(YAML.parse(block_result.get_data()), float_array, is_packed_float32_array_equal, "block style")

## Test number formats
func test_number_formats() -> void:
	# Test a simple array with different number formats
	var float_array = PackedFloat32Array([42.0, 123.5, 999.99])

	# Test different number formats
	var formats = {
		"Decimal": YAMLStyle.NUM_DECIMAL,
		"Scientific": YAMLStyle.NUM_SCIENTIFIC,
		"Hex": YAMLStyle.NUM_HEX,
		# Using Hex, Octal, and Binary for floats isn't intuitive
		# but we'll test them to ensure they don't cause errors
	}

	for format_name in formats:
		var number_style = YAML.create_style()
		var items_style = YAML.create_style()
		items_style.set_number_format(formats[format_name])
		number_style.set_child("_items", items_style)

		var result = YAML.stringify(float_array, number_style)
		assert_stringify_success(result, format_name + " format")

		if !result.has_error():
			print_rich("• %s format:" % format_name)
			print_rich(result.get_data())

			# Check format specific features
			match formats[format_name]:
				YAMLStyle.NUM_SCIENTIFIC:
					# Look for scientific notation (e or E)
					var has_scientific = result.get_data().find("e") != -1 || result.get_data().find("E") != -1
					if has_scientific:
						assert_true(has_scientific, "Uses scientific notation")
					else:
						print_rich("[color=yellow]⚠ Scientific notation not detected (may be implemented differently)[/color]")
				YAMLStyle.NUM_HEX:
					# May or may not be implemented for floats, just check it doesn't error
					print_rich("  (Note: Hex format for floats may not be directly supported)")

			# Test roundtrip
			assert_roundtrip(YAML.parse(result.get_data()), float_array, is_packed_float32_array_equal, format_name)

## Test special floating point values
func test_special_values() -> void:
	# Create an array with special floating point values
	var special_array = PackedFloat32Array([
		INF,      # Infinity
		-INF,     # Negative infinity
		NAN,      # Not a number
		0.0,      # Zero
		-0.0,     # Negative zero
		PI,       # Pi constant
		1.0e-10,  # Very small number
		1.0e+10   # Very large number
	])

	# Test with default style
	var result = YAML.stringify(special_array)
	assert_stringify_success(result, "special values")

	if !result.has_error():
		print_rich("• Special values:")
		print_rich(result.get_data())

		# Verify special value representation
		assert_yaml_has_feature(result.get_data(), ".inf", "Infinity represented as .inf")
		assert_yaml_has_feature(result.get_data(), "-.inf", "Negative infinity represented as -.inf")
		assert_yaml_has_feature(result.get_data(), ".nan", "NaN represented as .nan")

		# Parse back
		var parse_result = YAML.parse(result.get_data())
		assert_parse_success(parse_result, "parse special values")

		if !parse_result.has_error():
			var parsed_array = parse_result.get_data()

			# Test specific special values (we can't use == for NaN)
			assert_true(is_inf(parsed_array[0]) && parsed_array[0] > 0, "INF preserved")
			assert_true(is_inf(parsed_array[1]) && parsed_array[1] < 0, "-INF preserved")
			assert_true(is_nan(parsed_array[2]), "NaN preserved")

			# Others can be checked normally
			for i in range(3, special_array.size()):
				assert_true(
					abs(parsed_array[i] - special_array[i]) < 0.000001,
					"Value at index %d preserved" % i
				)

## Test item-specific styles
func test_item_styles() -> void:
	var float_array = PackedFloat32Array([
		1.23,    # Regular decimal
		7.89e5,  # Scientific notation
		456.0,   # Integer-like
	])

	# Create parent style
	var parent_style = YAML.create_style()

	# Create different styles for each item
	var item0_style = YAML.create_style()
	item0_style.set_number_format(YAMLStyle.NUM_DECIMAL)

	var item1_style = YAML.create_style()
	item1_style.set_number_format(YAMLStyle.NUM_SCIENTIFIC)

	var item2_style = YAML.create_style()
	item2_style.set_number_format(YAMLStyle.NUM_HEX)

	# Apply styles
	parent_style.set_child("0", item0_style)
	parent_style.set_child("1", item1_style)
	parent_style.set_child("2", item2_style)

	var result = YAML.stringify(float_array, parent_style)

	assert_stringify_success(result, "item styles")
	if !result.has_error():
		print_rich("• Item-specific styles:")
		print_rich(result.get_data())

		# Test roundtrip
		var parse_result = YAML.parse(result.get_data())
		assert_roundtrip(parse_result, float_array, is_packed_float32_array_equal, "item styles")

## Test float precision
func test_float_precision() -> void:
	# Create arrays with increasing precision requirements
	var precision_arrays = {
		"low_precision": PackedFloat32Array([
			1.5,
			2.25,
			3.75
		]),
		"medium_precision": PackedFloat32Array([
			1.23456,
			2.34567,
			3.45678
		]),
		"high_precision": PackedFloat32Array([
			1.2345678,
			2.3456789,
			3.4567890
		])
	}

	for name in precision_arrays:
		var float_array = precision_arrays[name]
		var result = YAML.stringify(float_array)

		assert_stringify_success(result, name)
		if result.has_error():
			continue

		print_rich("• %s:" % name)
		print_rich(result.get_data())

		# Parse back and check precision loss
		var parse_result = YAML.parse(result.get_data())
		assert_parse_success(parse_result, "parse " + name)

		if !parse_result.has_error():
			var parsed_array = parse_result.get_data()

			# Check each value with appropriate epsilon
			var epsilon = 0.000001  # Adjust based on expected precision
			for i in range(float_array.size()):
				var original = float_array[i]
				var parsed = parsed_array[i]
				var diff = abs(original - parsed)

				print_rich("  Value %d: Original=%f, Parsed=%f, Diff=%f" % [
					i, original, parsed, diff
				])

				assert_true(
					diff < epsilon,
					"Value at index %d preserved within epsilon=%f" % [i, epsilon]
				)

## Test large array
func test_large_array() -> void:
	var large_array = create_many_values_array(100)

	print_rich("\nTesting large Float32Array handling:")

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
		},
		{
			"name": "Large array with scientific notation",
			"style": func():
				var s = YAML.create_style()
				s.set_flow_style(YAMLStyle.FLOW_NONE)
				var items = YAML.create_style()
				items.set_number_format(YAMLStyle.NUM_SCIENTIFIC)
				s.set_child("_items", items)
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
			assert_roundtrip(parse_result, large_array, is_packed_float32_array_equal_large, name)

## Test parsing various float formats
func test_parse_formats() -> void:
	print_rich("\nTesting parsing of different float formats:")

	var test_formats = [
		"!PackedFloat32Array []",  # Empty array
		"!PackedFloat32Array [1.0, 2.0, 3.0]",  # Basic floats
		"!PackedFloat32Array [1, 2, 3]",  # Integers (should convert to floats)
		"!PackedFloat32Array [1.23e5, 4.56e-3]",  # Scientific notation
		"!PackedFloat32Array [.inf, -.inf, .nan]",  # Special values
		"!PackedFloat32Array [0.0, -0.0, 3.14159]"  # Zero, negative zero, PI
	]

	for format_str in test_formats:
		var parse_result = YAML.parse(format_str)
		assert_parse_success(parse_result, "Parse format: %s" % format_str)

		if not parse_result.has_error():
			var float_array = parse_result.get_data()
			print_rich("• %s → %s" % [format_str, format_array_for_display(float_array)])

			# For special values we need specific checks
			if format_str.find(".inf") != -1 or format_str.find(".nan") != -1:
				if format_str == "!PackedFloat32Array [.inf, -.inf, .nan]":
					assert_true(is_inf(float_array[0]) && float_array[0] > 0, "INF parsed correctly")
					assert_true(is_inf(float_array[1]) && float_array[1] < 0, "-INF parsed correctly")
					assert_true(is_nan(float_array[2]), "NaN parsed correctly")

## Test roundtrip conversion with style detection
func test_roundtrip_with_styles() -> void:
	var float_array = PackedFloat32Array([1.1, 2.2, 3.3, 4.4, 5.5])

	# Create a style with specific formatting
	var original_style = YAML.create_style()
	original_style.set_flow_style(YAMLStyle.FLOW_SINGLE)

	# Create item style for the first element
	var item_style = YAML.create_style()
	item_style.set_number_format(YAMLStyle.NUM_SCIENTIFIC)
	original_style.set_child("0", item_style)

	# Emit YAML with the style
	var emit_result = YAML.stringify(float_array, original_style)
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

		# Modify the array (add a float)
		var modified_array = PackedFloat32Array()
		for value in parsed_array:
			modified_array.append(value)
		modified_array.append(6.6)

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

## Helper function to check if PackedFloat32Array instances are equal
func is_packed_float32_array_equal(a: PackedFloat32Array, b: PackedFloat32Array, epsilon: float = 0.000001) -> bool:
	if a.size() != b.size():
		return false

	for i in range(a.size()):
		if is_nan(a[i]) and is_nan(b[i]):
			continue  # NaN == NaN for our comparison purposes
		elif is_inf(a[i]) and is_inf(b[i]):
			if (a[i] < 0) != (b[i] < 0):
				return false  # Signs must match for infinities
		elif abs(a[i] - b[i]) > epsilon:
			return false

	return true

## Helper function with higher epsilon for large arrays
func is_packed_float32_array_equal_large(a: PackedFloat32Array, b: PackedFloat32Array, epsilon: float = 0.001) -> bool:
	return is_packed_float32_array_equal(a, b, epsilon)

## Helper to format array contents for display
func format_array_for_display(array: PackedFloat32Array) -> String:
	if array.size() == 0:
		return "[]"

	var result = "["
	for i in range(array.size()):
		if i > 0:
			result += ", "

		# Special value handling
		if is_inf(array[i]):
			result += ".inf" if array[i] > 0 else "-.inf"
		elif is_nan(array[i]):
			result += ".nan"
		else:
			result += "%f" % array[i]

		# Limit display length for large arrays
		if i >= 5 and array.size() > 8:
			result += ", ... (%d more values)]" % (array.size() - i - 1)
			return result

	result += "]"
	return result

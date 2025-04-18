extends YAMLTest
## Test suite for PackedVector3Array YAML serialization and styling

func _init():
	test_name = "🧩 PackedVector3Array YAML Tests"

# Helper function to create test arrays with various Vector3 patterns
func create_test_arrays() -> Dictionary:
	return {
		"empty": PackedVector3Array(),
		"single": PackedVector3Array([Vector3(1, 2, 3)]),
		"basic": PackedVector3Array([
			Vector3(10, 20, 30),
			Vector3(40, 50, 60),
			Vector3(70, 80, 90)
		]),
		"negative": PackedVector3Array([
			Vector3(-1, -2, -3),
			Vector3(-4, -5, -6),
			Vector3(-7, -8, -9)
		]),
		"mixed": PackedVector3Array([
			Vector3(1, -2, 3),
			Vector3(-4, 5, -6),
			Vector3(7, -8, 9)
		]),
		"decimal": PackedVector3Array([
			Vector3(0.5, 1.5, 2.5),
			Vector3(3.5, 4.5, 5.5),
			Vector3(6.5, 7.5, 8.5)
		]),
		"constants": PackedVector3Array([
			Vector3.ZERO,
			Vector3.ONE,
			Vector3.LEFT,
			Vector3.RIGHT,
			Vector3.UP,
			Vector3.DOWN
		]),
		"normalized": PackedVector3Array([
			Vector3(3, 4, 5).normalized(),
			Vector3(-5, 12, 9).normalized(),
			Vector3(0, 1, 0).normalized()
		]),
		"large": create_large_vector2_array(50)
	}

# Helper to create a large array of Vector3s
func create_large_vector2_array(size: int) -> PackedVector3Array:
	var array = PackedVector3Array()
	array.resize(size)

	for i in range(size):
		array[i] = Vector3(
			sin(float(i) * 0.1) * 100,
			cos(float(i) * 0.1) * 100,
			tan(float(i) * 0.1) * 100
		)

	return array

## Test basic serialization/deserialization without styles
func test_basic_serialization() -> void:
	var test_arrays = create_test_arrays()

	for name in test_arrays:
		var vec_array = test_arrays[name]
		var result = YAML.stringify(vec_array)

		assert_stringify_success(result, name)
		if result.has_error():
			continue

		print_rich("• %s (%d elements): %s" % [
			name,
			vec_array.size(),
			truncate(result.get_data())
		])

		# Parse back and verify
		var parse_result = YAML.parse(result.get_data())
		assert_roundtrip(parse_result, vec_array, is_packed_vector2_array_equal, name)

## Test different flow styles (block vs flow)
func test_flow_styles() -> void:
	# Use a simple array for testing styles
	var vec_array = PackedVector3Array([
		Vector3(10, 20, 30),
		Vector3(40, 50, 60),
		Vector3(70, 80, 90)
	])

	# Test flow style (compact)
	var flow_style = YAML.create_style()
	flow_style.set_flow_style(YAMLStyle.FLOW_SINGLE)
	var flow_result = YAML.stringify(vec_array, flow_style)

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
	var block_result = YAML.stringify(vec_array, block_style)

	assert_stringify_success(block_result, "block style")
	if !block_result.has_error():
		print_rich("• Block style:")
		print_rich(block_result.get_data())

		# Verify it uses proper block style for a sequence
		assert_yaml_has_feature(block_result.get_data(), "- ", "Contains block sequence indicators")

	# Test roundtrip for both styles
	assert_roundtrip(YAML.parse(flow_result.get_data()), vec_array, is_packed_vector2_array_equal, "flow style")
	assert_roundtrip(YAML.parse(block_result.get_data()), vec_array, is_packed_vector2_array_equal, "block style")

## Test item-specific styles
func test_item_styles() -> void:
	var vec_array = PackedVector3Array([
		Vector3(10, 20, 30),
		Vector3(40, 50, 60),
		Vector3(70, 80, 90)
	])

	# Create parent style
	var parent_style = YAML.create_style()

	# Create special style for all items
	var items_style = YAML.create_style()
	items_style.set_flow_style(YAMLStyle.FLOW_SINGLE)  # Use flow style for vector2 elements

	# Create specific style for one item
	var item0_style = YAML.create_style()
	item0_style.set_container_form(YAMLStyle.FORM_SEQ)  # First item as sequence

	var item1_style = YAML.create_style()
	item1_style.set_container_form(YAMLStyle.FORM_MAP)  # Second item as map

	# Apply styles
	parent_style.set_child("_items", items_style)  # Apply to all items
	parent_style.set_child("0", item0_style)       # Apply to first item
	parent_style.set_child("1", item1_style)       # Apply to second item

	var result = YAML.stringify(vec_array, parent_style)

	assert_stringify_success(result, "item styles")
	if !result.has_error():
		print_rich("• Item-specific styles:")
		print_rich(result.get_data())

	# Test roundtrip
	assert_roundtrip(YAML.parse(result.get_data()), vec_array, is_packed_vector2_array_equal, "item styles")

## Test _items container forms
func test_items_container_forms() -> void:
	# Create a vector array
	var vec_array = PackedVector3Array([
		Vector3(1.5, 2.5, 3.5),
		Vector3(4.5, 5.5, 6.5)
	])

	# Test with FORM_SEQ for items
	var seq_style = YAML.create_style()
	var items_seq_style = YAML.create_style()
	items_seq_style.set_container_form(YAMLStyle.FORM_SEQ)
	seq_style.set_child("_items", items_seq_style)

	var seq_result = YAML.stringify(vec_array, seq_style)
	assert_stringify_success(seq_result, "array with items in sequence form")

	if !seq_result.has_error():
		print_rich("• Array with FORM_SEQ for items:")
		print_rich(seq_result.get_data())

		# Verify the YAML contains sequence indicators for items (absence of x:, y:, etc.)
		assert_yaml_lacks_feature(seq_result.get_data(), "x:", "Items use sequence form (no 'x:' key)")
		assert_yaml_lacks_feature(seq_result.get_data(), "y:", "Items use sequence form (no 'y:' key)")

		# Check if it has the sequence form (nested arrays)
		var has_nested_sequence = false
		var lines = seq_result.get_data().split("\n")
		for line in lines:
			# Look for lines that have nested sequence indicators
			if line.find("- - ") != -1 or (line.find("- [") != -1):
				has_nested_sequence = true
				break

		assert_true(has_nested_sequence, "Items contain nested sequence indicators")

	# Test with FORM_MAP for items (default, but let's be explicit)
	var map_style = YAML.create_style()
	var items_map_style = YAML.create_style()
	items_map_style.set_container_form(YAMLStyle.FORM_MAP)
	map_style.set_child("_items", items_map_style)

	var map_result = YAML.stringify(vec_array, map_style)
	assert_stringify_success(map_result, "array with items in map form")

	if !map_result.has_error():
		print_rich("• Array with FORM_MAP for items:")
		print_rich(map_result.get_data())

		# Verify the YAML contains map indicators for items
		assert_yaml_has_feature(map_result.get_data(), "x:", "Items use map form ('x:' key present)")
		assert_yaml_has_feature(map_result.get_data(), "y:", "Items use map form ('y:' key present)")

	# Test roundtrip for both forms
	assert_roundtrip(YAML.parse(seq_result.get_data()), vec_array, is_packed_vector2_array_equal, "items with FORM_SEQ")
	assert_roundtrip(YAML.parse(map_result.get_data()), vec_array, is_packed_vector2_array_equal, "items with FORM_MAP")

## Test flow styles for array items
func test_item_flow_styles() -> void:
	var vec_array = PackedVector3Array([
		Vector3(10, 20, 30),
		Vector3(40, 50, 60),
		Vector3(70, 80, 90)
	])

	# Create a style with flow style for vector items
	var style = YAML.create_style()

	# Set flow style for the array itself to block
	style.set_flow_style(YAMLStyle.FLOW_NONE)

	# But make each Vector3 item use flow style
	var items_style = YAML.create_style()
	items_style.set_flow_style(YAMLStyle.FLOW_SINGLE)
	style.set_child("_items", items_style)

	var result = YAML.stringify(vec_array, style)
	assert_stringify_success(result, "array with flow style items")

	if !result.has_error():
		print_rich("• Array with flow style items:")
		print_rich(result.get_data())

		# Verify the structure - array should be block style but items should be flow style
		assert_yaml_has_feature(result.get_data(), "- {", "Vector3 items use flow style")

		# Test roundtrip
		assert_roundtrip(YAML.parse(result.get_data()), vec_array, is_packed_vector2_array_equal, "array with flow items")

## Test parsing various Vector3Array formats
func test_parse_formats() -> void:
	print_rich("\nTesting parsing of different Vector3Array formats:")

	var test_formats = [
		"!PackedVector3Array []",  # Empty array
		"!PackedVector3Array [{x: 1, y: 2, z: 3}]",  # Single element with map format
		"!PackedVector3Array [[1, 2, 3]]",  # Single element with sequence format
		"!PackedVector3Array [{x: 1, y: 2, z: 3}, {x: 4, y: 5, z: 6}]",  # Multiple elements with map format
		"!PackedVector3Array [[1, 2, 3], [4, 5, 6]]",  # Multiple elements with sequence format
		"!PackedVector3Array [{x: 1.5, y: 2.5, z: 3.5}, [4.5, 5.5, 6.5]]"  # Mixed format (should still work)
	]

	for format_str in test_formats:
		var parse_result = YAML.parse(format_str)
		assert_parse_success(parse_result, "Parse format: %s" % format_str)

		if not parse_result.has_error():
			var vec_array = parse_result.get_data()
			print_rich("• %s → %s" % [format_str, format_array_for_display(vec_array)])

## Test array with large number of elements
func test_large_array() -> void:
	var large_array = create_large_vector2_array(100)

	print_rich("\nTesting large Vector3Array handling:")

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
			"name": "Large array with flow style items",
			"style": func():
				var s = YAML.create_style()
				s.set_flow_style(YAMLStyle.FLOW_NONE)
				var items = YAML.create_style()
				items.set_flow_style(YAMLStyle.FLOW_SINGLE)
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

			# Parse back and verify
			var parse_result = YAML.parse(yaml_str)
			assert_roundtrip(parse_result, large_array, is_packed_vector2_array_equal, name)

## Test roundtrip conversion with style detection
func test_roundtrip_with_styles() -> void:
	var vec_array = PackedVector3Array([
		Vector3(10, 20, 30),
		Vector3(40, 50, 60),
		Vector3(70, 80, 90)
	])

	# Create a style with specific formatting
	var original_style = YAML.create_style()
	original_style.set_flow_style(YAMLStyle.FLOW_SINGLE)

	# Create item style for the first element
	var item_style = YAML.create_style()
	item_style.set_container_form(YAMLStyle.FORM_MAP)
	original_style.set_child("0", item_style)

	# Emit YAML with the style
	var emit_result = YAML.stringify(vec_array, original_style)
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

		# Modify the array (add a vector)
		var modified_array = PackedVector3Array()
		for vector in parsed_array:
			modified_array.append(vector)
		modified_array.append(Vector3(70, 80, 90))

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

## Helper function to check if PackedVector3Array instances are equal
func is_packed_vector2_array_equal(a: PackedVector3Array, b: PackedVector3Array, epsilon: float = 0.00001) -> bool:
	if a.size() != b.size():
		return false

	for i in range(a.size()):
		if !a[i].is_equal_approx(b[i]):
			return false

	return true

## Helper to format array contents for display
func format_array_for_display(array: PackedVector3Array) -> String:
	if array.size() == 0:
		return "[]"

	var result = "["
	for i in range(array.size()):
		if i > 0:
			result += ", "
		result += "(%f, %f)" % [array[i].x, array[i].y]

		# Limit display length for large arrays
		if i >= 3 and array.size() > 5:
			result += ", ... (%d more vectors)]" % (array.size() - i - 1)
			return result

	result += "]"
	return result

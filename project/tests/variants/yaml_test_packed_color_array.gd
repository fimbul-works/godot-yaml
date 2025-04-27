extends YAMLTest
## Test suite for PackedColorArray YAML serialization and styling

func _init():
	test_name = "PackedColorArray"

# Helper function to create test arrays
func create_test_arrays() -> Dictionary:
	return {
		"Empty": PackedColorArray(),
		"Single": PackedColorArray([Color.RED]),
		"Basic": PackedColorArray([Color.RED, Color.GREEN, Color.BLUE]),
		"Named Colors": PackedColorArray([
			Color.WHITE, Color.BLACK, Color.RED, Color.GREEN,
			Color.BLUE, Color.YELLOW, Color.MAGENTA, Color.CYAN
		]),
		"Hex Colors": PackedColorArray([
			Color("#ff0000"), Color("#00ff00"), Color("#0000ff"),
			Color("#ffff00"), Color("#ff00ff"), Color("#00ffff")
		]),
		"Transparent": PackedColorArray([
			Color(1, 0, 0, 0),    # Transparent red
			Color(0, 1, 0, 0.25), # 25% green
			Color(0, 0, 1, 0.5),  # 50% blue
			Color(1, 1, 0, 0.75)  # 75% yellow
		]),
		"Decimal": PackedColorArray([
			Color(0.1, 0.2, 0.3, 1.0),
			Color(0.4, 0.5, 0.6, 0.7),
			Color(0.8, 0.9, 1.0, 0.5)
		]),
		"HSV_derived": PackedColorArray([
			Color.from_hsv(0.0, 1.0, 1.0),   # Red
			Color.from_hsv(0.33, 1.0, 1.0),  # Green
			Color.from_hsv(0.67, 1.0, 1.0),  # Blue
			Color.from_hsv(0.5, 0.5, 0.5)    # Muted cyan
		])
	}

## Test basic serialization/deserialization without styles
func test_basic_serialization() -> void:
	var test_arrays = create_test_arrays()

	for name in test_arrays:
		var color_array = test_arrays[name]
		var result = YAML.stringify(color_array)

		assert_stringify_success(result, name)
		if result.has_error():
			continue

		print_rich("• %s (%d elements): %s" % [
			name,
			color_array.size(),
			truncate(result.get_data(), 60)
		])

		# Parse back and verify
		var parse_result = YAML.parse(result.get_data())
		assert_roundtrip(parse_result, color_array, is_packed_color_array_equal, name)

## Test different flow styles (block vs flow)
func test_flow_styles() -> void:
	# Use a simple array for testing styles
	var color_array = PackedColorArray([Color.RED, Color.GREEN, Color.BLUE])

	# Test flow style
	var flow_result = YAML.stringify(color_array, YAML.create_style().set_flow_style(YAMLStyle.FLOW_SINGLE))

	assert_stringify_success(flow_result, "Flow style")
	if !flow_result.has_error():
		print_rich("• Flow style:")
		print_rich(flow_result.get_data())

		# Verify it contains flow indicators (brackets)
		assert_yaml_has_feature(flow_result.get_data(), "[", "Contains opening bracket")
		assert_yaml_has_feature(flow_result.get_data(), "]", "Contains closing bracket")
	assert_roundtrip(YAML.parse(flow_result.get_data()), color_array, is_packed_color_array_equal, "Flow style")

	# Test block style
	var block_result = YAML.stringify(color_array, YAML.create_style().set_flow_style(YAMLStyle.FLOW_NONE))

	assert_stringify_success(block_result, "Block style")
	if !block_result.has_error():
		print_rich("• Block style:")
		print_rich(block_result.get_data())

		# Verify it uses proper block style for a sequence
		assert_yaml_has_feature(block_result.get_data(), "- ", "Contains block sequence indicators")
	assert_roundtrip(YAML.parse(block_result.get_data()), color_array, is_packed_color_array_equal, "Block style")

## Test item-specific styles
func test_item_styles() -> void:
	var color_array = PackedColorArray([
		Color.RED,
		Color.GREEN,
		Color.BLUE
	])

	# Create parent style
	var parent_style = YAML.create_style()

	# Create special style for all items
	var template = YAML.create_style().set_binary_encoding(YAMLStyle.BIN_HEX) # Use hex representation for colors

	# Create specific style for one item
	var item0_style = YAML.create_style()
	item0_style.set_string_style(YAMLStyle.STRING_QUOTE_DOUBLE)

	# Apply styles
	parent_style.set_child("_template", template)  # Apply to all items
	parent_style.set_child("0", item0_style)       # Apply to first item

	var result = YAML.stringify(color_array, parent_style)

	assert_stringify_success(result, "Item styles")
	if !result.has_error():
		print_rich("• Item-specific styles:")
		print_rich(result.get_data())

		# Check if hex format is used (# symbols)
		assert_yaml_has_feature(result.get_data(), "#", "Uses hex color format")

	# Test roundtrip
	assert_roundtrip(YAML.parse(result.get_data()), color_array, is_packed_color_array_equal, "Item styles")

## Test _template child style with FORM_SEQ and FORM_MAP
func test_template_container_forms() -> void:
	# Create a color array with colors that have multiple components
	var color_array = PackedColorArray([
		Color(1.0, 0.5, 0.2, 0.8),  # Orange with alpha
		Color(0.2, 0.8, 1.0, 0.6)   # Light blue with alpha
	])

	# Test with FORM_SEQ for items
	var seq_style = YAML.create_style().set_child("_template", YAML.create_style().set_container_form(YAMLStyle.FORM_SEQ))

	var seq_result = YAML.stringify(color_array, seq_style)
	assert_stringify_success(seq_result, "array with items in sequence form")

	if !seq_result.has_error():
		print_rich("• Array with FORM_SEQ for items:")
		print_rich(seq_result.get_data())

		# Verify the YAML contains sequence indicators for items (absence of r:, g:, etc.)
		assert_yaml_lacks_feature(seq_result.get_data(), "r:", "Items use sequence form (no 'r' key)")
		assert_yaml_lacks_feature(seq_result.get_data(), "g:", "Items use sequence form (no 'g' key)")
		assert_yaml_lacks_feature(seq_result.get_data(), "b:", "Items use sequence form (no 'b' key)")
		assert_yaml_lacks_feature(seq_result.get_data(), "a:", "Items use sequence form (no 'a' key)")

	assert_roundtrip(YAML.parse(seq_result.get_data()), color_array, is_packed_color_array_equal, "Items with FORM_SEQ")

	# Test with FORM_MAP for items (default, but let's be explicit)
	var map_style = YAML.create_style().set_child("_template", YAML.create_style().set_container_form(YAMLStyle.FORM_MAP))

	var map_result = YAML.stringify(color_array, map_style)
	assert_stringify_success(map_result, "Array with items in map form")

	if !map_result.has_error():
		print_rich("• Array with FORM_MAP for items:")
		print_rich(map_result.get_data())

		# Verify the YAML contains map indicators for items
		assert_yaml_has_feature(map_result.get_data(), "r:", "Items use map form ('r:' key present)")
		assert_yaml_has_feature(map_result.get_data(), "g:", "Items use map form ('g:' key present)")
		assert_yaml_has_feature(map_result.get_data(), "b:", "Items use map form ('b:' key present)")
		assert_yaml_has_feature(map_result.get_data(), "a:", "Items use map form ('a:' key present)")

	assert_roundtrip(YAML.parse(map_result.get_data()), color_array, is_packed_color_array_equal, "Items with FORM_MAP")

## Test array with color formats
func test_color_formats() -> void:
	# Create an array with colors in different formats
	var color_array = PackedColorArray([
		Color(1.0, 0.0, 0.0),         # Red as RGB floats
		Color("#00FF00"),             # Green as hex string
		Color.from_hsv(0.5, 1.0, 1.0) # Cyan from HSV
	])

	# Test different styles for representing these colors
	var styles = {
		"Default": null,  # No specific style
		"Hex Number": func():
			var s = YAML.create_style()
			var template = YAML.create_style()
			template.set_integer_format(YAMLStyle.INT_HEX)
			s.set_child("_template", template)
			return s,
		"Hex String": func():
			var s = YAML.create_style()
			var template = YAML.create_style()
			template.set_binary_encoding(YAMLStyle.BIN_HEX)
			s.set_child("_template", template)
			return s,
		"RGB Map": func():
			var s = YAML.create_style()
			var template = YAML.create_style()
			template.set_container_form(YAMLStyle.FORM_MAP)
			s.set_child("_template", template)
			return s,
		"RGB Sequence": func():
			var s = YAML.create_style()
			var template = YAML.create_style()
			template.set_container_form(YAMLStyle.FORM_SEQ)
			s.set_child("_template", template)
			return s
	}

	for style_name in styles:
		var style_func = styles[style_name]
		var style = style_func.call() if style_func else null

		var result = YAML.stringify(color_array, style)
		assert_stringify_success(result, style_name + " format")

		if !result.has_error():
			print_rich("• %s format:" % style_name)
			print_rich(result.get_data())

			# Test roundtrip
			var parse_result = YAML.parse(result.get_data())
			assert_roundtrip(parse_result, color_array, is_packed_color_array_equal, style_name + " format")

## Test transparency handling
func test_transparency() -> void:
	# Create an array with transparencies
	var transparent_array = PackedColorArray([
		Color(1, 0, 0, 1),    # Opaque red
		Color(0, 1, 0, 0.5),  # Semi-transparent green
		Color(0, 0, 1, 0),    # Fully transparent blue
	])

	var result = YAML.stringify(transparent_array)
	assert_stringify_success(result, "transparency array")

	if !result.has_error():
		print_rich("• Transparency array:")
		print_rich(result.get_data())

		# Parse back and verify alpha channels are preserved
		var parse_result = YAML.parse(result.get_data())
		assert_parse_success(parse_result, "parse transparency array")

		if !parse_result.has_error():
			var parsed_array = parse_result.get_data()
			assert_equal(parsed_array[0].a, 1.0, "Opaque alpha preserved")
			assert_equal(parsed_array[1].a, 0.5, "Semi-transparent alpha preserved")
			assert_equal(parsed_array[2].a, 0.0, "Fully transparent alpha preserved")

## Test roundtrip conversion with style detection enabled
func test_roundtrip_with_styles() -> void:
	var color_array = PackedColorArray([Color.RED, Color.GREEN, Color.BLUE])

	# Create a style with specific formatting
	var original_style = YAML.create_style()
	original_style.set_flow_style(YAMLStyle.FLOW_SINGLE)

	# Emit YAML with the style
	var emit_result = YAML.stringify(color_array, original_style)
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
		var detected_style = parse_result.get_style()
		var parsed_array = parse_result.get_data()

		# Modify the array (add a color)
		var modified_array = PackedColorArray()
		for color in parsed_array:
			modified_array.append(color)
		modified_array.append(Color.YELLOW)

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
	# Test PackedColorArray-specific parsing errors
	var invalid_color_array = """
!PackedColorArray
- {r: 1.0, g: 0.0}
"""
	assert_parse_error(invalid_color_array, "Missing color fields detection")

	var negative_color_values = """
!PackedColorArray
- {r: -1.0, g: 0.0, b: 0.0}
"""
	var parse_result = YAML.parse(negative_color_values)
	if !parse_result.has_error():
		# Colors should be more than 0.0
		var array = parse_result.get_data()
		assert_false(array[0].r < 0.0, "Color values should be positive")

## Helper function to check if PackedColorArray instances are equal
func is_packed_color_array_equal(a: PackedColorArray, b: PackedColorArray, epsilon: float = 0.01) -> bool:
	if a.size() != b.size():
		return false

	for i in range(a.size()):
		var color_a = a[i]
		var color_b = b[i]

		if abs(color_a.r - color_b.r) > epsilon || abs(color_a.g - color_b.g) > epsilon || abs(color_a.b - color_b.b) > epsilon || abs(color_a.a - color_b.a) > epsilon:
			return false

	return true

extends YAMLTest
## Test suite for PackedColorArray YAML serialization and styling

func _init():
	test_name = "🧩 PackedColorArray YAML Tests"

# Helper function to create test arrays
func create_test_arrays() -> Dictionary:
	return {
		"empty": PackedColorArray(),
		"single": PackedColorArray([Color.RED]),
		"basic": PackedColorArray([Color.RED, Color.GREEN, Color.BLUE]),
		"named_colors": PackedColorArray([
			Color.WHITE, Color.BLACK, Color.RED, Color.GREEN,
			Color.BLUE, Color.YELLOW, Color.MAGENTA, Color.CYAN
		]),
		"hex_colors": PackedColorArray([
			Color("#ff0000"), Color("#00ff00"), Color("#0000ff"),
			Color("#ffff00"), Color("#ff00ff"), Color("#00ffff")
		]),
		"transparent": PackedColorArray([
			Color(1, 0, 0, 0),    # Transparent red
			Color(0, 1, 0, 0.25), # 25% green
			Color(0, 0, 1, 0.5),  # 50% blue
			Color(1, 1, 0, 0.75)  # 75% yellow
		]),
		"decimal": PackedColorArray([
			Color(0.1, 0.2, 0.3, 1.0),
			Color(0.4, 0.5, 0.6, 0.7),
			Color(0.8, 0.9, 1.0, 0.5)
		]),
		"hsv_derived": PackedColorArray([
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
			result.get_data().substr(0, 60) + ("..." if result.get_data().length() > 60 else "")
		])

		# Parse back and verify
		var parse_result = YAML.parse(result.get_data())
		assert_roundtrip(parse_result, color_array, is_packed_color_array_equal, name)

## Test different flow styles (block vs flow)
func test_flow_styles() -> void:
	# Use a simple array for testing styles
	var color_array = PackedColorArray([Color.RED, Color.GREEN, Color.BLUE])

	# Test flow style (compact)
	var flow_style = YAML.create_style()
	flow_style.set_flow_style(YAMLStyle.FLOW_SINGLE)
	var flow_result = YAML.stringify(color_array, flow_style)

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
	var block_result = YAML.stringify(color_array, block_style)

	assert_stringify_success(block_result, "block style")
	if !block_result.has_error():
		print_rich("• Block style:")
		print_rich(block_result.get_data())

		# Verify it uses proper block style for a sequence
		assert_yaml_has_feature(block_result.get_data(), "- ", "Contains block sequence indicators")

	# Test roundtrip for both styles
	assert_roundtrip(YAML.parse(flow_result.get_data()), color_array, is_packed_color_array_equal, "flow style")
	assert_roundtrip(YAML.parse(block_result.get_data()), color_array, is_packed_color_array_equal, "block style")

## Test item-specific styles
func test_item_styles() -> void:
	var color_array = PackedColorArray([Color.RED, Color.GREEN, Color.BLUE])

	# Create parent style
	var parent_style = YAML.create_style()

	# Create special style for all items
	var items_style = YAML.create_style()
	items_style.set_binary_encoding(YAMLStyle.BIN_HEX)  # Use hex representation for colors

	# Create specific style for one item
	var item0_style = YAML.create_style()
	item0_style.set_quote_style(YAMLStyle.QUOTE_DOUBLE)

	# Apply styles
	parent_style.set_child("_items", items_style)  # Apply to all items
	parent_style.set_child("0", item0_style)       # Apply to first item

	var result = YAML.stringify(color_array, parent_style)

	assert_stringify_success(result, "item styles")
	if !result.has_error():
		print_rich("• Item-specific styles:")
		print_rich(result.get_data())

		# Check if hex format is used (# symbols)
		assert_yaml_has_feature(result.get_data(), "#", "Uses hex color format")

	# Test roundtrip
	assert_roundtrip(YAML.parse(result.get_data()), color_array, is_packed_color_array_equal, "item styles")

## Test _items child style with FORM_SEQ and FORM_MAP
func test_items_container_forms() -> void:
	# Create a color array with colors that have multiple components
	var color_array = PackedColorArray([
		Color(1.0, 0.5, 0.2, 0.8),  # Orange with alpha
		Color(0.2, 0.8, 1.0, 0.6)   # Light blue with alpha
	])

	# Test with FORM_SEQ for items
	var seq_style = YAML.create_style()
	var items_seq_style = YAML.create_style()
	items_seq_style.set_container_form(YAMLStyle.FORM_SEQ)
	seq_style.set_child("_items", items_seq_style)

	var seq_result = YAML.stringify(color_array, seq_style)
	assert_stringify_success(seq_result, "array with items in sequence form")

	if !seq_result.has_error():
		print_rich("• Array with FORM_SEQ for items:")
		print_rich(seq_result.get_data())

		# Verify the YAML contains sequence indicators for items (absence of r:, g:, etc.)
		assert_yaml_lacks_feature(seq_result.get_data(), "r:", "Items use sequence form (no 'r:' key)")
		assert_yaml_lacks_feature(seq_result.get_data(), "g:", "Items use sequence form (no 'g:' key)")

		# Should instead have dash indicators within items
		var has_nested_sequence = false
		var lines = seq_result.get_data().split("\n")
		for line in lines:
			# Look for lines that have nested sequence indicators
			if line.find("- - ") != -1 or (line.find("- ") != -1 and line.find(":") == -1):
				has_nested_sequence = true
				break

		assert_true(has_nested_sequence, "Items contain nested sequence indicators")

	# Test with FORM_MAP for items (default, but let's be explicit)
	var map_style = YAML.create_style()
	var items_map_style = YAML.create_style()
	items_map_style.set_container_form(YAMLStyle.FORM_MAP)
	map_style.set_child("_items", items_map_style)

	var map_result = YAML.stringify(color_array, map_style)
	assert_stringify_success(map_result, "array with items in map form")

	if !map_result.has_error():
		print_rich("• Array with FORM_MAP for items:")
		print_rich(map_result.get_data())

		# Verify the YAML contains map indicators for items
		assert_yaml_has_feature(map_result.get_data(), "r:", "Items use map form ('r:' key present)")
		assert_yaml_has_feature(map_result.get_data(), "g:", "Items use map form ('g:' key present)")

	# Test roundtrip for both forms
	assert_roundtrip(YAML.parse(seq_result.get_data()), color_array, is_packed_color_array_equal, "items with FORM_SEQ")
	assert_roundtrip(YAML.parse(map_result.get_data()), color_array, is_packed_color_array_equal, "items with FORM_MAP")

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
		"Hex String": YAMLStyle.BIN_HEX,
		"Hex Number": YAMLStyle.NUM_HEX
	}

	for style_name in styles:
		var format_style = YAML.create_style()
		if styles[style_name] != null:
			if styles[style_name] == YAMLStyle.BIN_HEX:
				format_style.set_binary_encoding(styles[style_name])
			elif styles[style_name] == YAMLStyle.NUM_HEX:
				format_style.set_number_format(styles[style_name])

		var result = YAML.stringify(color_array, format_style)
		assert_stringify_success(result, style_name + " format")

		if !result.has_error():
			print_rich("• %s format:" % style_name)
			print_rich(result.get_data())

			# Test roundtrip
			var parse_result = YAML.parse(result.get_data())
			assert_roundtrip(parse_result, color_array, is_packed_color_array_equal, style_name + " format")

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

## Helper function to check if PackedColorArray instances are equal
func is_packed_color_array_equal(a: PackedColorArray, b: PackedColorArray, epsilon: float = 0.01) -> bool:
	if a.size() != b.size():
		return false

	for i in range(a.size()):
		if !a[i].is_equal_approx(b[i]):
			return false

	return true

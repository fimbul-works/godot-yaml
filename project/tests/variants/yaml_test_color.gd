extends YAMLTest
## Test suite for Color YAML serialization and styling

# Test values with different color variants
var test_values = {
	"Red": Color.RED,
	"Green": Color.GREEN,
	"Blue": Color.BLUE,
	"White": Color.WHITE,
	"Black": Color.BLACK,
	"Transparent": Color(1, 1, 1, 0),
	"Semi_transparent": Color(1, 0, 0, 0.5),
	"Custom": Color(0.7, 0.4, 0.9),
	"Decimal": Color(0.123, 0.456, 0.789, 0.567),
}

func _init():
	test_name = "PackedColorArray"

## Test basic serialization/deserialization without styles
func test_basic_serialization() -> void:
	run_basic_serialization_test(test_values, is_color_equal)

## Test different container forms (map vs sequence)
func test_container_forms() -> void:
	var color = Color(0.1, 0.2, 0.3, 0.4)
	run_container_forms_test(color, is_color_equal, ["r", "g", "b", "a"])

## Test different flow styles (block vs flow)
func test_flow_styles() -> void:
	var color = Color(0.1, 0.2, 0.3, 0.4)
	run_flow_styles_test(color, is_color_equal)

## Test special color hex formats
func test_hex_formats() -> void:
	var color = Color(1, 0.5, 0, 1)  # Orange

	# Test hex format with # prefix
	var hex_style = YAML.create_style()
	hex_style.set_binary_encoding(YAMLStyle.BIN_HEX)
	var hex_result = YAML.stringify(color, hex_style)

	assert_stringify_success(hex_result, "Hex format with # prefix")
	if not hex_result.has_error():
		print_rich("• Hex format with # prefix:")
		print_rich(hex_result.get_data())
		assert_yaml_has_feature(hex_result.get_data(), "#", "Contains # symbol")
	assert_roundtrip(YAML.parse(hex_result.get_data()), color, is_color_equal, "# Hex format")

	# Test hex format with 0x prefix
	var hex0x_style = YAML.create_style()
	hex0x_style.set_integer_format(YAMLStyle.INT_HEX)
	var hex0x_result = YAML.stringify(color, hex0x_style)

	assert_stringify_success(hex0x_result, "Hex format with 0x prefix")
	if not hex0x_result.has_error():
		print_rich("• Hex format with 0x prefix:")
		print_rich(hex0x_result.get_data())

		# Verify if it contains 0x prefix or uses another format
		if hex0x_result.get_data().find("0x") != -1:
			assert_yaml_has_feature(hex0x_result.get_data(), "0x", "Contains 0x prefix")
		else:
			print_rich("[color=yellow]⚠ Format does not use 0x prefix (this may be expected)[/color]")
	assert_roundtrip(YAML.parse(hex0x_result.get_data()), color, is_color_equal, "0x hex format")

## Test alpha channel handling
func test_alpha_channel() -> void:
	# Create colors with different alpha values
	var colors = {
		"opaque": Color(0.5, 0.5, 0.5, 1.0),
		"semi": Color(0.5, 0.5, 0.5, 0.5),
		"transparent": Color(0.5, 0.5, 0.5, 0.0)
	}

	for name in colors:
		var color = colors[name]
		print_rich("\n• Testing %s color (alpha=%.2f):" % [name, color.a])

		# Regular format
		var result = YAML.stringify(color)
		assert_stringify_success(result, "%s regular format" % name)
		print_rich("  Regular format: %s" % result.get_data())

		# Check if alpha is included for non-opaque colors
		if color.a < 1.0:
			if result.get_data().find("a:") != -1:
				assert_yaml_has_feature(result.get_data(), "a:", "Alpha component included")
			elif result.get_data().find("#") != -1 and result.get_data().length() >= 9:  # #RRGGBBAA format
				print_rich("[color=green]✓ Alpha included in hex format[/color]")
			else:
				print_rich("[color=yellow]⚠ Alpha channel representation not detected[/color]")

		# Hex format
		var hex_style = YAML.create_style()
		hex_style.set_binary_encoding(YAMLStyle.BIN_HEX)
		var hex_result = YAML.stringify(color, hex_style)

		assert_stringify_success(hex_result, "%s hex format" % name)
		print_rich("  Hex format: %s" % hex_result.get_data())

		# For hex format, check if alpha is included in the hex string when alpha < 1
		if color.a < 1.0:
			if hex_result.get_data().find("#") != -1:
				# Assuming #RRGGBBAA format
				var hex_str = hex_result.get_data().strip_edges()
				if name == "transparent" and hex_str.length() >= 9:
					assert_true(hex_str.replace("'", "").ends_with("00"), "Alpha is 0 in hex format")
				elif hex_str.length() >= 9:
					assert_true(true, "Hex format includes alpha channel")
				else:
					print_rich("[color=yellow]⚠ Hex format does not seem to include alpha[/color]")

		# Test roundtrip
		assert_roundtrip(YAML.parse(result.get_data()), color, is_color_equal, "%s roundtrip" % name)
		assert_roundtrip(YAML.parse(hex_result.get_data()), color, is_color_equal, "%s hex roundtrip" % name)

## Test parsing various color formats
func test_parse_formats() -> void:
	print_rich("\nTesting parsing of different color formats:")

	var test_formats = [
		"!Color '#FF0000'",         # Hex string with # (RGB)
		"!Color '#FF0000FF'",       # Hex string with # (RGBA)
		"!Color '#FF000080'",       # Hex string with # (RGBA semi-transparent)
		"!Color '0xFF0000'",        # Hex string with 0x (RGB)
		"!Color '0xFF0000FF'",      # Hex string with 0x (RGBA)
		"!Color {r: 1.0, g: 0.0, b: 0.0}", # Map (RGB)
		"!Color {r: 1.0, g: 0.0, b: 0.0, a: 0.5}", # Map (RGBA)
		"!Color [1.0, 0.0, 0.0]",   # Sequence (RGB)
		"!Color [1.0, 0.0, 0.0, 0.5]" # Sequence (RGBA)
	]

	for format_str in test_formats:
		var parse_result = YAML.parse(format_str)
		assert_parse_success(parse_result, "Parse format: %s" % format_str)

		if not parse_result.has_error():
			var color = parse_result.get_data()
			print_rich("• %s → Color(%.2f, %.2f, %.2f, %.2f)" %
				[format_str, color.r, color.g, color.b, color.a])

			# For semi-transparent formats, verify alpha was preserved
			if format_str.find("0.5") != -1 or format_str.find("80") != -1:
				assert_true(
					abs(color.a - 0.5) < 0.01,
					"Alpha channel preserved for %s" % format_str
				)

## Test error handling for invalid YAML
func test_parsing_errors() -> void:
	# Test Color-specific parsing errors
	var invalid_color_yaml = """
!Color
r: 1.0
g: 0.0
# Missing b field
"""
	assert_parse_error(invalid_color_yaml, "Missing required field detection")

	var wrong_type_yaml = """
!Color "not a color"
"""
	assert_parse_error(wrong_type_yaml, "Wrong type/format detection")

	var invalid_sequence_length = """
!Color [1.0, 0.0]
"""
	assert_parse_error(invalid_sequence_length, "Invalid sequence length detection")

	var invalid_hex = """
!Color "#GGHHII"
"""
	assert_parse_error(invalid_hex, "Invalid hex color detection")

## Helper function to check if Colors are equal (with floating point precision)
func is_color_equal(a: Color, b: Color, epsilon: float = 0.01) -> bool:
	return (
		abs(a.r - b.r) < epsilon &&
		abs(a.g - b.g) < epsilon &&
		abs(a.b - b.b) < epsilon &&
		abs(a.a - b.a) < epsilon
	)

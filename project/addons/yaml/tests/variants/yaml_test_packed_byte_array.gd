extends YAMLTest
## Test suite for PackedByteArray YAML serialization and styling

func _init():
	test_name = "🧩 PackedByteArray YAML Tests"

# Helper function to create test arrays with various data patterns
func create_test_arrays() -> Dictionary:
	return {
		"empty": PackedByteArray(),
		"single": PackedByteArray([42]),
		"small": PackedByteArray([0, 1, 2, 3, 4]),
		"basic": PackedByteArray([10, 20, 30, 40, 50]),
		"ascii": "Hello, World!".to_utf8_buffer(),
		"all_bytes": create_all_bytes_array(),
		"random": create_random_bytes(32),
		"binary_data": PackedByteArray([0xFF, 0x00, 0xAA, 0x55, 0x0F, 0xF0]),
		"large": create_random_bytes(256),
		"sequential": create_sequential_array(100)
	}

# Helper to create an array containing all byte values (0-255)
func create_all_bytes_array() -> PackedByteArray:
	var array = PackedByteArray()
	array.resize(256)
	for i in range(256):
		array[i] = i
	return array

# Helper to create array with random bytes
func create_random_bytes(size: int) -> PackedByteArray:
	var array = PackedByteArray()
	array.resize(size)

	for i in range(size):
		array[i] = randi() % 256

	return array

# Helper to create a sequential array
func create_sequential_array(size: int) -> PackedByteArray:
	var array = PackedByteArray()
	array.resize(size)

	for i in range(size):
		array[i] = i % 256

	return array

## Test basic serialization/deserialization without styles
func test_basic_serialization() -> void:
	var test_arrays = create_test_arrays()

	for name in test_arrays:
		var byte_array = test_arrays[name]
		var result = YAML.stringify(byte_array)

		assert_stringify_success(result, name)
		if result.has_error():
			continue

		print_rich("• %s (%d bytes): %s" % [
			name,
			byte_array.size(),
			truncate(result.get_data())
		])

		# Parse back and verify
		var parse_result = YAML.parse(result.get_data())
		assert_roundtrip(parse_result, byte_array, is_packed_byte_array_equal, name)

## Test base64 encoding format
func test_base64_encoding() -> void:
	# Use several different arrays to test base64 encoding
	var test_arrays = {
		"ascii": "Hello, World!".to_utf8_buffer(),
		"binary": PackedByteArray([0xFF, 0x00, 0xAA, 0x55, 0x0F, 0xF0]),
		"random": create_random_bytes(32)
	}

	# Create base64 style
	var base64_style = YAML.create_style()
	base64_style.set_binary_encoding(YAMLStyle.BIN_BASE64)

	for name in test_arrays:
		var byte_array = test_arrays[name]
		var result = YAML.stringify(byte_array, base64_style)

		assert_stringify_success(result, "base64 - " + name)
		if result.has_error():
			continue

		print_rich("• Base64 - %s: %s" % [name, truncate(result.get_data())])

		# Verify it looks like base64 (no 0x or hex chars, potential = padding)
		var data = result.get_data()
		var looks_like_base64 = data.find("0x") == -1 && !contains_only_hex_chars(data)
		assert_true(looks_like_base64, "Output uses base64 encoding")

		# Parse back and verify
		var parse_result = YAML.parse(result.get_data())
		assert_roundtrip(parse_result, byte_array, is_packed_byte_array_equal, "base64 - " + name)

## Test hex encoding format
func test_hex_encoding() -> void:
	# Use several different arrays to test hex encoding
	var test_arrays = {
		"ascii": "Hello, World!".to_utf8_buffer(),
		"binary": PackedByteArray([0xFF, 0x00, 0xAA, 0x55, 0x0F, 0xF0]),
		"random": create_random_bytes(16)
	}

	# Create hex style
	var hex_style = YAML.create_style()
	hex_style.set_binary_encoding(YAMLStyle.BIN_HEX)

	for name in test_arrays:
		var byte_array = test_arrays[name]
		var result = YAML.stringify(byte_array, hex_style)

		assert_stringify_success(result, "hex - " + name)
		if result.has_error():
			continue

		print_rich("• Hex - %s: %s" % [name, truncate(result.get_data())])

		# Verify it looks like hex (only hex chars, no base64 padding)
		var data = result.get_data().strip_edges()
		var looks_like_hex = contains_only_hex_chars(data) && data.find("=") == -1
		assert_true(looks_like_hex, "Output uses hex encoding")

		# Parse back and verify
		var parse_result = YAML.parse(result.get_data())
		assert_roundtrip(parse_result, byte_array, is_packed_byte_array_equal, "hex - " + name)

## Test different scalar styles with binary data
func test_scalar_styles() -> void:
	# Use a medium-sized array to test line wrapping
	var byte_array = create_sequential_array(64)

	# Test literal block style for better readability
	var literal_style = YAML.create_style()
	literal_style.set_scalar_style(YAMLStyle.SCALAR_LITERAL)
	literal_style.set_binary_encoding(YAMLStyle.BIN_BASE64)

	var literal_result = YAML.stringify(byte_array, literal_style)
	assert_stringify_success(literal_result, "literal block style")

	if !literal_result.has_error():
		print_rich("• Literal block style (base64):")
		print_rich(literal_result.get_data())

		# Verify it contains the literal style indicator
		assert_yaml_has_feature(literal_result.get_data(), "|\n", "Contains literal block style indicator")

		# Verify it contains newlines for formatting
		assert_true(literal_result.get_data().find("\n ") != -1, "Contains line breaks in the encoded data")

	# Test folded block style
	var folded_style = YAML.create_style()
	folded_style.set_scalar_style(YAMLStyle.SCALAR_FOLDED)
	folded_style.set_binary_encoding(YAMLStyle.BIN_HEX)

	var folded_result = YAML.stringify(byte_array, folded_style)
	assert_stringify_success(folded_result, "folded block style")

	if !folded_result.has_error():
		print_rich("• Folded block style (hex):")
		print_rich(folded_result.get_data())

		# Verify it contains the folded style indicator
		assert_yaml_has_feature(folded_result.get_data(), ">\n", "Contains folded block style indicator")

	# Test plain scalar style
	var plain_style = YAML.create_style()
	plain_style.set_scalar_style(YAMLStyle.SCALAR_PLAIN)
	plain_style.set_binary_encoding(YAMLStyle.BIN_BASE64)

	var plain_result = YAML.stringify(byte_array, plain_style)
	assert_stringify_success(plain_result, "plain scalar style")

	# Test roundtrip for all styles
	assert_roundtrip(YAML.parse(literal_result.get_data()), byte_array, is_packed_byte_array_equal, "literal style")
	assert_roundtrip(YAML.parse(folded_result.get_data()), byte_array, is_packed_byte_array_equal, "folded style")
	assert_roundtrip(YAML.parse(plain_result.get_data()), byte_array, is_packed_byte_array_equal, "plain style")

## Test parsing various binary data formats
func test_parse_formats() -> void:
	print_rich("\nTesting parsing of different binary data formats:")

	# Define our test bytes
	var test_bytes = PackedByteArray([0x48, 0x65, 0x6C, 0x6C, 0x6F])  # "Hello" in ASCII

	# Base64 encoding of "Hello"
	var base64_hello = "SGVsbG8="
	# Hex encoding of "Hello"
	var hex_hello = "48656C6C6F"

	var test_formats = [
		"!PackedByteArray '%s'" % base64_hello,  # Base64 with single quotes
		"!PackedByteArray \"%s\"" % base64_hello,  # Base64 with double quotes
		"!PackedByteArray '%s'" % hex_hello,  # Hex with single quotes
		"!PackedByteArray \"%s\"" % hex_hello,  # Hex with double quotes
		"!PackedByteArray |\n  %s" % base64_hello,  # Base64 with literal block
		"!PackedByteArray >\n  %s" % hex_hello,  # Hex with folded block
	]

	for format_str in test_formats:
		var parse_result = YAML.parse(format_str)
		assert_parse_success(parse_result, "Parse format: %s" % format_str)

		if not parse_result.has_error():
			var byte_array = parse_result.get_data()
			# For better readability, convert small arrays to hex representation
			var hex_repr = byte_array_to_hex_string(byte_array)
			print_rich("• %s → %s" % [format_str, hex_repr])

			# Verify the parsed result matches our expected bytes
			assert_true(
				is_packed_byte_array_equal(byte_array, test_bytes),
				"Correctly parsed binary data from format: %s" % format_str
			)

## Test large binary data handling
func test_large_binary_data() -> void:
	# Create a larger array that will require line wrapping
	var large_array = create_sequential_array(500)

	print_rich("\nTesting large binary data handling:")

	# Test with different encodings and block styles
	var test_cases = [
		{
			"name": "Large base64 in literal block",
			"style": func():
				var s = YAML.create_style()
				s.set_binary_encoding(YAMLStyle.BIN_BASE64)
				s.set_scalar_style(YAMLStyle.SCALAR_LITERAL)
				return s,
		},
		{
			"name": "Large hex in literal block",
			"style": func():
				var s = YAML.create_style()
				s.set_binary_encoding(YAMLStyle.BIN_HEX)
				s.set_scalar_style(YAMLStyle.SCALAR_LITERAL)
				return s,
		},
		{
			"name": "Large base64 in plain style",
			"style": func():
				var s = YAML.create_style()
				s.set_binary_encoding(YAMLStyle.BIN_BASE64)
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
			print_rich("  First 60 chars: " + yaml_str.substr(0, 60) + "...")

			# Check if it contains newlines for proper formatting
			var has_newlines = yaml_str.find("\n") != -1
			assert_true(has_newlines, "Large data is formatted with newlines")

			# Parse back and verify
			var parse_result = YAML.parse(yaml_str)
			assert_roundtrip(parse_result, large_array, is_packed_byte_array_equal, name)

## Test roundtrip conversion with style detection
func test_roundtrip_with_styles() -> void:
	var byte_array = "StyleDetectionTest".to_utf8_buffer()

	# Create a style with specific formatting
	var original_style = YAML.create_style()
	original_style.set_scalar_style(YAMLStyle.SCALAR_LITERAL)
	original_style.set_binary_encoding(YAMLStyle.BIN_HEX)

	# Emit YAML with the style
	var emit_result = YAML.stringify(byte_array, original_style)
	assert_stringify_success(emit_result, "initial stringify")
	if emit_result.has_error():
		return

	var yaml_text = emit_result.get_data()
	print_rich("• Original YAML (with hex encoding and literal style):")
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

		# Modify the array (add more bytes)
		var modified_array = PackedByteArray()
		for byte in parsed_array:
			modified_array.append(byte)
		modified_array.append_array("_appended".to_utf8_buffer())

		# Re-emit with the detected style
		var re_emit_result = YAML.stringify(modified_array, detected_style)
		assert_stringify_success(re_emit_result, "re-stringify with detected style")
		if re_emit_result.has_error():
			return

		print_rich("• Re-emitted YAML (with preserved style):")
		print_rich(re_emit_result.get_data())

		# Verify the style was preserved
		assert_yaml_has_feature(re_emit_result.get_data(), "|", "Literal style was preserved")
		# Since we're using hex encoding, check if output contains only hex characters
		assert_true(contains_only_hex_chars(re_emit_result.get_data()), "Hex encoding was preserved")

## Helper function to check if PackedByteArray instances are equal
func is_packed_byte_array_equal(a: PackedByteArray, b: PackedByteArray) -> bool:
	if a.size() != b.size():
		return false

	for i in range(a.size()):
		if a[i] != b[i]:
			return false

	return true

## Helper to check if a string contains only hex characters (ignoring whitespace and block indicators)
func contains_only_hex_chars(text: String) -> bool:
	var cleaned = ""
	for c in text.replace("!PackedByteArray", ""):
		# Skip whitespace, quotes, and YAML block indicators
		if c == ' ' or c == '\n' or c == '\t' or c == "'" or c == '"' or c == '|' or c == '>' or c == "-":
			continue
		cleaned += c

	# Check each remaining character
	for c in cleaned:
		if not (c >= '0' and c <= '9') and not (c.to_lower() >= 'a' and c.to_lower() <= 'f'):
			return false

	return true

## Helper to convert a PackedByteArray to a hex string for display
func byte_array_to_hex_string(array: PackedByteArray) -> String:
	if array.size() == 0:
		return "[]"

	var result = "["
	for i in range(array.size()):
		if i > 0:
			result += ", "
		result += "0x%02X" % array[i]

		# Limit display length for large arrays
		if i >= 16 and array.size() > 20:
			result += ", ... (%d more bytes)]" % (array.size() - i - 1)
			return result

	result += "]"
	return result
